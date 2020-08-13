#pragma once

#include <ostream>

namespace LightControl {
	class SerializableMessage {
	public:
		/**
		 * @brief serialises the message so it complies with the protocol
		 * @returns ostringstream buffer containing the message.
		 */

		virtual std::shared_ptr<std::ostringstream> compose() const = 0;
		virtual ~SerializableMessage() {}

	};
}