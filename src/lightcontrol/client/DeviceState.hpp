#pragma once

#include <boost/asio/buffer.hpp>

#include "messages/PulseMessage.hpp"
#include "messages/StrobeMessage.hpp"
#include "messages/RainbowMessage.hpp"
#include "messages/GlitterMessage.hpp"
#include "messages/NodeInfoMessage.hpp"
#include "messages/LightModeMessage.hpp"
#include "messages/OnBoardLEDStrength.hpp"
#include "messages/RunningSectionsMessage.hpp"


namespace LightControl {
	class DeviceState {
	public:
		/**
		 * @brief parses the incomming message and alters the state
		 *
		 * @param[in] buffer data received in message.
		 */
		void parseMessage(boost::asio::mutable_buffer& buffer);

		/** Received states */
		PulseMessage pulse;
		StrobeMessage strobe;
		RainbowMessage rainbow;
		GlitterMessage glitter;
		NodeInfoMessage nodeInfo;
		LightModeMessage lightMode;
		OnBoardLEDStrength onboardLed;
		RunningSectionsMessage runningSections;
	};
}