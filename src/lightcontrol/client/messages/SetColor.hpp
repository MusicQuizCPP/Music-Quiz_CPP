#pragma once

#include <boost/asio/buffer.hpp>

#include <cstdint>

#include "SerializableMessage.hpp"
#include <inttypes.h>
#include "lightcontrol/client/DeviceState.hpp"

namespace LightControl {
	class SetColor : public SerializableMessage {
	public:
		/**
		 * @brief default Constructor
		 */
		SetColor() {};

		/**
		 * @brief Constructor
		 *
		 * @param[in] r red.
		 * @param[in] g green.
		 * @param[in] b blue.

		 */
		SetColor(uint8_t r, uint8_t g, uint8_t b)
			: _r(r), _g(g), _b(b)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 * @returns std::string shared_ptr containing the serialized message.
		 */

		virtual std::shared_ptr<std::string> compose() const;

	private:
		uint8_t _r;
		uint8_t _g;
		uint8_t _b;
	};
}

