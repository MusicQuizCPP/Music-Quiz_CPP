#pragma once

#include <boost/asio/buffer.hpp>

#include <memory>
#include <string>
#include <cstdint>

#include "SerializableMessage.hpp"
#include <inttypes.h>
#include "lightcontrol/client/DeviceState.hpp"


namespace LightControl {
	class SetOn : public SerializableMessage {
	public:
		/**
		 * @brief default Constructor
		 */
		SetOn() {}

		/**
		 * @brief Constructor
		 *
		 * @param[in] on should the light be on.
		 * @param[in] brightness Brightness (0-255),

		 */
		SetOn(bool on, uint8_t brightness)
			: _on(on), _brightness(brightness)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 * @returns std::string shared_ptr containing the serialized message.
		 */

		virtual std::shared_ptr<std::string> compose() const;

	private:
		bool _on;
		uint8_t _brightness;
	};
}

