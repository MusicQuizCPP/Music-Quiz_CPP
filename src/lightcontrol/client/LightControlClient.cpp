#include "LightControlClient.hpp"

#include <chrono>
#include <iostream>

#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast/websocket/stream_base.hpp>

#include "common/Log.hpp"
#include "DeviceState.hpp"


LightControl::LightControlClient::LightControlClient(std::string hostname, uint16_t port) :
	_port(port), _hostname(hostname),
	_resolver(boost::asio::make_strand(_ioContext)),
	_ws(boost::asio::make_strand(_ioContext)),
	_state(std::make_unique<DeviceState>())
{}

void LightControl::LightControlClient::start()
{
	if ( !_connectThread ) {
		_connectThread = new std::thread(&LightControlClient::run, this);
	}
}

LightControl::LightControlClient::~LightControlClient()
{
	_shouldStop = true;
	_ioContext.stop();
	_connectThread->join();
	delete _connectThread;
}

void LightControl::LightControlClient::run()
{
	while ( !_shouldStop && _hostname != "" ) {
		_isRunning = true;

		_resolver.async_resolve(_hostname.c_str(), std::to_string(_port).c_str(),
			boost::beast::bind_front_handler(&LightControlClient::on_resolve, shared_from_this()));
		_ioContext.run();

		_isConnected = false;
		_ioContext.restart();
	}

	_isRunning = false;
}

void LightControl::LightControlClient::on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
{
	if ( ec ) {
		LOG_WARN("Resolve failed");
		return;
	}

	/** Set timeout for connect */
	boost::beast::get_lowest_layer(_ws).expires_after(std::chrono::seconds(5));
	_ws.next_layer().async_connect(results, boost::beast::bind_front_handler(&LightControlClient::on_connect, shared_from_this()));
}

void LightControl::LightControlClient::on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type ep)
{
	boost::ignore_unused(ep);

	if ( ec ) {
		LOG_WARN("Connect failed");
		return;
	}

	/** Turn off the timeout on the tcp_stream, because the websocket stream has its own timeout system. */
	boost::beast::get_lowest_layer(_ws).expires_never();

	/** Set suggested timeout settings for the websocket */
	boost::beast::websocket::stream_base::timeout opt{
		std::chrono::seconds(5),   // handshake timeout
		std::chrono::seconds(10),  // idle timeout
		true,
	};

	_ws.set_option(opt);

	/** Set a decorator to change the User-Agent of the handshake */
	_ws.set_option(boost::beast::websocket::stream_base::decorator(
		[](boost::beast::websocket::request_type& req) {
		req.set(boost::beast::http::field::user_agent,
			"MusicQuiz");
	}));

	/** Update the host string. This will provide the value of the Host HTTP header during the WebSocket handshake. See https://tools.ietf.org/html/rfc7230#section-5.4 */
	std::string host_header = _hostname + ":" + std::to_string(_port);

	/** Perform the websocket handshake */
	_ws.async_handshake(host_header, "/ws", boost::beast::bind_front_handler(&LightControlClient::on_handshake, shared_from_this()));
}

void LightControl::LightControlClient::on_handshake(boost::beast::error_code ec)
{
	if ( ec ) {
		LOG_WARN("Handshake failed");
		return;
	}

	_isConnected = true;

	for ( auto& callback : _connectedCallbacks ) {
		callback(this);
	}
	_ws.async_read(_readBuffer, boost::beast::bind_front_handler(&LightControlClient::on_read, shared_from_this()));
}

void LightControl::LightControlClient::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);
	if ( ec ) {
		return;
	}

	boost::asio::mutable_buffer data = _readBuffer.data();
	parseMessage(data);

	_readBuffer.clear();

	if ( !_shouldStop ) {
		_ws.async_read(_readBuffer, boost::beast::bind_front_handler(&LightControlClient::on_read, shared_from_this()));
	}
}

void LightControl::LightControlClient::parseMessage(boost::asio::mutable_buffer& buffer)
{
	if ( !buffer.size() ) {
		LOG_ERROR("Received empty message!");
		return;
	}

	const std::lock_guard<std::mutex> lock(_stateLock);

	_state->parseMessage(buffer);
}

std::string LightControl::LightControlClient::getConnectionString()
{
	const std::lock_guard<std::mutex> lock(_stateLock);
	if ( !_isConnected || _state->name.size() == 0 ) {
		return "Disconnected";
	} else {
		std::string conStr = "Connected to " + _state->name + "\n";
		return conStr;
	}
}

void LightControl::LightControlClient::sendMessage(const SerializableMessage& msg, std::chrono::milliseconds timeout)
{
	sendMessage(msg.compose(), timeout);
}

void LightControl::LightControlClient::sendMessage(std::shared_ptr<std::string> data, std::chrono::milliseconds timeout)
{
	const std::lock_guard<std::mutex> lock(_writeLock);

	if ( !_sendingMessage ) {
		_sendingMessage = true;
		_ws.async_write(boost::asio::buffer(*data), boost::beast::bind_front_handler(&LightControlClient::on_write, shared_from_this()));
	} else {
		_writeQueue.push(PendingMessage(data, timeout));
	}
}

void LightControl::LightControlClient::on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
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
			_ws.async_write(boost::asio::buffer(*_writeBuffer), boost::beast::bind_front_handler(&LightControlClient::on_write, shared_from_this()));
			break;
		}
	}
}