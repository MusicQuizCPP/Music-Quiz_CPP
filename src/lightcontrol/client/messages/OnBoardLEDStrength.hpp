#pragma once

#include <boost/asio/buffer.hpp>

#include <chrono>

#include "LightModes.hpp"
#include "PackageTypes.hpp"
#include "SerializableMessage.hpp"


namespace LightControl {
	class OnBoardLEDStrength : public SerializableMessage {

	public:
		/**
		 * @brief default Constructor
		 */
		OnBoardLEDStrength() = default;

		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer buffer containing message data.
		 */
		OnBoardLEDStrength(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief Constructor
		 *
		 * @param[in] strength Strenght of the onboard debug LED (0-1).
		 */
		OnBoardLEDStrength(float strength)
			: _strength(strength)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 *
		 * @returns ostringstream shared_ptr containing the serialized message.
		 */
		virtual std::shared_ptr<std::ostringstream> compose() const;

	private:
		float _strength;
	};
}