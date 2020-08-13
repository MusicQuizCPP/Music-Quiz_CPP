#pragma once

#include <boost/asio/buffer.hpp>

#include <cstdint>

#include "LightModes.hpp"
#include "PackageTypes.hpp"
#include "SerializableMessage.hpp"


namespace LightControl {
	class LightModeMessage : public SerializableMessage {
	public:
		/**
		 * @brief default Constructor
		 */
		LightModeMessage() {};

		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer buffer containing message data.
		 */
		LightModeMessage(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief Constructor
		 *
		 * @param[in] mode Light mode to set.
		 * @param[in] strength strengt of light (0-1).
		 * @param[in] r red colour.
		 * @param[in] g green color.
		 * @param[in] b blue color.
		 */
		LightModeMessage(LightMode mode, float strength, uint8_t r, uint8_t g, uint8_t b)
			: _mode(mode), _strength(strength), _r(r), _g(g), _b(b)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 *
		 * @returns ostringstream shared_ptr containing the serialized message.
		 */

		virtual std::shared_ptr<std::ostringstream> compose() const;

	private:
		uint8_t _r;
		uint8_t _g;
		uint8_t _b;
		LightMode _mode;
		float _strength;
	};
}

