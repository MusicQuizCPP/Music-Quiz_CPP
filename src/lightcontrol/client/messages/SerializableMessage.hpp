#pragma once

#include <ostream>

#include "lightcontrol/client/DeviceState.hpp"

namespace LightControl {
	class SerializableMessage {
	public:
		/**
		 * @brief serialises the message so it complies with the protocol
		 * @param[in] state device state.
		 * @returns std::string containing the message.
		 */

		virtual std::shared_ptr<std::string> compose() const = 0;
		virtual ~SerializableMessage() {}

	};
}