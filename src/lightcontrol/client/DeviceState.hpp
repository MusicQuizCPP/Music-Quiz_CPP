#pragma once

#include <boost/asio/buffer.hpp>
#include <string>
#include <vector>
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
		std::vector<std::string> effects;
		std::vector<std::string> palettes;

	};
}