#pragma once

#include <boost/asio/buffer.hpp>
#include <string>

namespace LightControl {
	class DeviceState {
	public:
		/**
		 * @brief parses the incomming message and alters the state
		 *
		 * @param[in] buffer data received in message.
		 */
		void parseMessage(boost::asio::mutable_buffer& buffer);

		std::string name;
	};
}