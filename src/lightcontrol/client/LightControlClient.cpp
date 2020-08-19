#include "DeviceState.hpp"
#include "LightControlClient.hpp"
#include "messages/PackageTypes.hpp"

#include <chrono>
#include <iostream>

#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast/websocket/stream_base.hpp>

#include "common/Log.hpp"


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using namespace LightControl;
using namespace std::chrono_literals;


LightControlClient::LightControlClient(std::string hostname, uint16_t port) :
	_port(port), _hostname(hostname),
	_resolver(net::make_strand(_ioContext)),
	_ws(net::make_strand(_ioContext)),
	_state(std::make_unique<DeviceState>())
{}

void LightControlClient::start()
{
	if ( !_connectThread ) {
		_connectThread = new std::thread(&LightControlClient::run, this);
	}
}

LightControlClient::~LightControlClient()
{
	_shouldStop = true;
	_ioContext.stop();
	_connectThread->join();
	delete _connectThread;
}

void LightControlClient::run()
{
	while ( !_shouldStop && _hostname != "" ) {
		_isRunning = true;

		_resolver.async_resolve(_hostname.c_str(), std::to_string(_port).c_str(),
			beast::bind_front_handler(&LightControlClient::on_resolve, shared_from_this()));
		_ioContext.run();

		_isConnected = false;
		_ioContext.restart();
	}

	_isRunning = false;
}

void LightControlClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
	if ( ec ) {
		LOG_WARN("Resolve failed");
		return;
	}

	/** Set timeout for connect */
	beast::get_lowest_layer(_ws).expires_after(std::chrono::seconds(5));
	_ws.next_layer().async_connect(results, beast::bind_front_handler(&LightControlClient::on_connect, shared_from_this()));
}

void LightControlClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
	boost::ignore_unused(ep);

	if ( ec ) {
		LOG_WARN("Connect failed");
		return;
	}

	/** Turn off the timeout on the tcp_stream, because the websocket stream has its own timeout system. */
	beast::get_lowest_layer(_ws).expires_never();

	/** Set suggested timeout settings for the websocket */
	websocket::stream_base::timeout opt{
		std::chrono::seconds(5),   // handshake timeout
		std::chrono::seconds(10),  // idle timeout
		true,
	};

	_ws.set_option(opt);

	/** Set a decorator to change the User-Agent of the handshake */
	_ws.set_option(websocket::stream_base::decorator(
		[](websocket::request_type& req) {
		req.set(http::field::user_agent,
			"MusicQuiz");
	}));

	/** Update the host string. This will provide the value of the Host HTTP header during the WebSocket handshake. */
	/** See https://tools.ietf.org/html/rfc7230#section-5.4 */
	std::string host_header = _hostname + ":" + std::to_string(_port);

	/** Perform the websocket handshake */
	_ws.async_handshake(host_header, "/", beast::bind_front_handler(&LightControlClient::on_handshake, shared_from_this()));
}

void LightControlClient::on_handshake(beast::error_code ec)
{
	if ( ec ) {
		LOG_WARN("Handshake failed");
		return;
	}

	_isConnected = true;

	for ( auto& callback : _connectedCallbacks ) {
		callback(this);
	}

	_ws.async_read(_readBuffer, beast::bind_front_handler(&LightControlClient::on_read, shared_from_this()));
}

void LightControlClient::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);
	if ( ec ) {
		return;
	}

	net::mutable_buffer data = _readBuffer.data();
	parseMessage(data);

	_readBuffer.clear();

	if ( !_shouldStop ) {
		_ws.async_read(_readBuffer, beast::bind_front_handler(&LightControlClient::on_read, shared_from_this()));
	}
}

void LightControlClient::parseMessage(net::mutable_buffer& buffer)
{
	if ( !buffer.size() ) {
		LOG_ERROR("Received empty message!");
		return;
	}

	const std::lock_guard<std::mutex> lock(_stateLock);

	_state->parseMessage(buffer);
}

std::string LightControlClient::getConnectionString()
{
	const std::lock_guard<std::mutex> lock(_stateLock);
	if ( !_isConnected || _state->nodeInfo.nodes.size() == 0 ) {
		return "Disconnected";
	} else {
		std::string conStr = "Connected to " + _state->nodeInfo.nodes[0].name + "\n";
		conStr += "Total devices: " + std::to_string(_state->nodeInfo.nodes.size());
		return conStr;
	}
}

void LightControlClient::sendMessage(const SerializableMessage& msg, std::chrono::milliseconds timeout)
{
	sendMessage(msg.compose(), timeout);
}

void LightControlClient::sendMessage(std::shared_ptr<std::ostringstream> data, std::chrono::milliseconds timeout)
{
	const std::lock_guard<std::mutex> lock(_writeLock);

	if ( !_sendingMessage ) {
		_writeBuffer = data;
		_sendingMessage = true;
		_ws.async_write(net::buffer(_writeBuffer->str()), beast::bind_front_handler(&LightControlClient::on_write, shared_from_this()));
	} else {
		_writeQueue.push(PendingMessage(data, timeout));
	}
}

void LightControlClient::on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(ec);
	boost::ignore_unused(bytes_transferred);
	LOG_DEBUG("Transfered " + std::to_string(bytes_transferred));

	const std::lock_guard<std::mutex> lock(_writeLock);

	_sendingMessage = false;

	while ( _writeQueue.size() ) {
		PendingMessage msg = _writeQueue.front();

		_writeQueue.pop();
		_writeBuffer = msg._data;

		if ( !msg.isTimedOut() ) {
			_sendingMessage = true;
			_ws.async_write(net::buffer(_writeBuffer->str()), beast::bind_front_handler(&LightControlClient::on_write, shared_from_this()));
			break;
		}
	}
}