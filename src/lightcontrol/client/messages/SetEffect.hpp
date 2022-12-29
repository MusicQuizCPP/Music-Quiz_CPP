#pragma once

#include <boost/asio/buffer.hpp>

#include <cstdint>

#include "SerializableMessage.hpp"
#include <inttypes.h>
#include "lightcontrol/client/DeviceState.hpp"

namespace LightControl {
	enum WledEffects {
		SOLID = 	   0,
		BLINK = 	   1,
		SPARKLE_PLUS = 22,
	};

	class SetEffect : public SerializableMessage {
	public:
		/**
		 * @brief default Constructor
		 */
		SetEffect() {};

		/**
		 * @brief Constructor
		 *
		 * @param[in] effect effect to set.
		 * @param[in] speed effect speed (0-255).
		 * @param[in] intensity effect intensity (0-255)

		 */
		SetEffect(WledEffects effect, uint8_t speed, uint8_t intensity)
			: _effect(effect), _speed(speed), _intensity(intensity)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 * @returns std::string shared_ptr containing the serialized message.
		 */

		virtual std::shared_ptr<std::string> compose() const;

	private:
		WledEffects _effect;
		uint8_t _speed;
		uint8_t _intensity;
	};
}

