#pragma once

#include <boost/asio/buffer.hpp>

#include <chrono>

#include "LightModes.hpp"
#include "PackageTypes.hpp"
#include "SerializableMessage.hpp"


namespace LightControl {
	class StrobeMessage : public SerializableMessage {
	public:
		/**
		 * @brief default Constructor
		 */
		StrobeMessage() = default;

		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer buffer containing message data.
		 */
		StrobeMessage(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief Constructor
		 *
		 * @param[in] onTime  time the LEDs are on.
		 * @param[in] offTime time the LEDs are off.
		 */
		StrobeMessage(std::chrono::microseconds onTime, std::chrono::microseconds offTime)
			: _onTime(onTime), _offTime(offTime)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 *
		 * @returns ostringstream shared_ptr containing the serialized message.
		 */
		virtual std::shared_ptr<std::ostringstream> compose() const;

	private:
		/** Variables */
		std::chrono::microseconds _onTime;
		std::chrono::microseconds _offTime;
	};
}