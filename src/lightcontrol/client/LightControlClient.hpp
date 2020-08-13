#pragma once

#include <string>
#include <cstdint>
#include <thread>
#include <mutex>
#include <queue>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>

#include "messages/PendingMessage.hpp"
#include "messages/SerializableMessage.hpp"


namespace LightControl {
	class DeviceState;

	class LightControlClient : public std::enable_shared_from_this<LightControlClient> {
		typedef void (*connected_callback)(LightControlClient*);

	public:

		/**
		 * @brief Constructor
		 *
		 * @param[in] hostname Device to connect to.
		 * @param[in] port port to connect to.
		 */
		LightControlClient(std::string hostname, uint16_t port);

		~LightControlClient();

		/**
		 * @brief Is the target device connected.
		 */
		bool isConnected() { return _isConnected; }

		/**
		 * @brief Is client running and attempting to connect or already connected.
		 */
		bool isRunning() { return _isRunning; }

		/**
		 * @brief Get string representation of connected device
		 */
		std::string getConnectionString(void);

		/**
		 * @brief queue a message for sending. The message will not be send imediatly, but be queued
		 *        A timeout can be given where the message will be discarded if it has not been delivered
		 *        to the OS network layer after a given time. Default is 1000 milliseconds
		 *
		 * @param[in] data message to send.
		 * @param[in] timeout how long the message can be queued before it times out.
		 */
		void sendMessage(std::shared_ptr<std::ostringstream> data, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));

		/**
		 * @brief queue a message for sending. The message will not be send imediatly, but be queued
		 *        A timeout can be given where the message will be discarded if it has not been delivered
		 *        to the OS network layer after a given time. Default is 1000 milliseconds
		 *
		 * @param[in] msg message to send.
		 * @param[in] timeout how long the message can be queued before it times out.
		 */
		void sendMessage(const SerializableMessage& msg, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));

		/**
		 * @brief start the client
		 */
		void start();

		/**
		 * @brief add callback to be called when a connection is established
		 *        must be called before start()
		 */

		void addConnectedCallback(connected_callback fptr)
		{
			_connectedCallbacks.push_back(fptr);
		}

	private:
		/**
		 * @brief Function for thread driving connection logic
		 */
		void run();

		/**
		 * @brief Parse the given buffer as a message
		 */
		void parseMessage(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief callback when hostname is resolved
		 */
		void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);

		/**
		 * @brief callback when connection established to host
		 */
		void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type ep);

		/**
		 * @brief callback when websocket handshake with host is done
		 */
		void on_handshake(boost::beast::error_code ec);

		/**
		 * @brief callback when a new message is read
		 */
		void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);

		/**
		 * @brief callback when a message is written
		 */
		void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);


		/** Variables */
		bool _shouldStop;
		bool _isRunning = false;
		bool _isConnected = false;
		bool _sendingMessage = false;

		uint16_t _port;
		std::string _hostname;
		boost::asio::io_context _ioContext;
		boost::asio::ip::tcp::resolver _resolver;
		boost::beast::websocket::stream<boost::beast::tcp_stream> _ws;

		std::thread* _connectThread = nullptr;
		std::vector<connected_callback> _connectedCallbacks;

		std::mutex  _writeLock;
		std::mutex  _stateLock;

		std::unique_ptr<DeviceState> _state;

		boost::beast::flat_buffer _readBuffer;
		std::queue<PendingMessage> _writeQueue;
		std::shared_ptr<std::ostringstream> _writeBuffer;
	};
}