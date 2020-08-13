#pragma once

#include <boost/asio/buffer.hpp>

#include <chrono>

#include "LightModes.hpp"
#include "PackageTypes.hpp"
#include "SerializableMessage.hpp"



namespace LightControl {
	class PulseMessage : public SerializableMessage {

	public:
		enum class PulseDirection {
			PULSE_OUT = -1,
			PULSE_NONE = 0,
			PULSE_IN = 1,
		};

		/**
		 * @brief default Constructor
		 */
		PulseMessage() = default;

		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer buffer containing message data.
		 */
		PulseMessage(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief Constructor
		 *
		 * @param[in] pulseTime Time the pusle should take to complete.
		 * @param[in] direction The direction of the pulse.
		 * @param[in] singleShot Whether the pulse should just run once, or continue
		 */
		PulseMessage(std::chrono::milliseconds pulseTime, PulseDirection direction, bool singleShot)
			: _pulseTime(pulseTime), _direction(direction), _singleShot(singleShot)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 *
		 * @returns ostringstream shared_ptr containing the serialized message.
		 */
		virtual std::shared_ptr<std::ostringstream> compose() const;

	private:
		/** Variables */
		bool _singleShot;
		PulseDirection _direction;
		std::chrono::milliseconds _pulseTime;

	};
}