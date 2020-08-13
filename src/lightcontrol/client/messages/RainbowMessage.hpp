#pragma once

#include <boost/asio/buffer.hpp>

#include <chrono>

#include "LightModes.hpp"
#include "PackageTypes.hpp"
#include "SerializableMessage.hpp"


namespace LightControl {
	class RainbowMessage : public SerializableMessage {

	public:
		/**
		 * @brief default Constructor
		 */
		RainbowMessage() = default;

		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer buffer containing message data.
		 */
		RainbowMessage(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief Constructor
		 *
		 * @param[in] updateRate how often the rainbow should be updated
		 */
		RainbowMessage(std::chrono::milliseconds updateRate)
			: _updateRate(updateRate)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 *
		 * @returns ostringstream shared_ptr containing the serialized message.
		 */
		virtual std::shared_ptr<std::ostringstream> compose() const;

	private:
		/** Variables */
		std::chrono::milliseconds _updateRate;
	};
}