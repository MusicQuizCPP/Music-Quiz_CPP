#pragma once

#include <boost/asio/buffer.hpp>

#include <chrono>

#include "LightModes.hpp"
#include "PackageTypes.hpp"
#include "SerializableMessage.hpp"


namespace LightControl {
	class GlitterMessage : public SerializableMessage {
	public:
		/**
		 * @brief default Constructor
		 */
		GlitterMessage() = default;

		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer buffer containing message data.
		 */
		GlitterMessage(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief Constructor
		 *
		 * @param[in] updateRate time between LED updates.
		 * @param[in] randomColor whether random colors should be used.
		 * @param[in] percentOn percent of LEDs to be on (0-100)
		 */
		GlitterMessage(std::chrono::microseconds updateRate, bool randomColor, float percentOn)
			: _updateRate(updateRate), _randomColor(randomColor), _percentOn(percentOn)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 *
		 * @returns ostringstream shared_ptr containing the serialized message.
		 */
		virtual std::shared_ptr<std::ostringstream> compose() const;

	private:
		float _percentOn;
		bool _randomColor;
		std::chrono::microseconds _updateRate;
	};
}