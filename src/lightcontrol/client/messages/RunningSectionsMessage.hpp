#pragma once

#include <boost/asio/buffer.hpp>

#include <chrono>

#include "LightModes.hpp"
#include "PackageTypes.hpp"
#include "SerializableMessage.hpp"


namespace LightControl {
	class RunningSectionsMessage : public SerializableMessage {

	public:
		/**
		 * @brief default Constructor
		 */
		RunningSectionsMessage() = default;

		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer buffer containing message data.
		 */
		RunningSectionsMessage(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief Constructor
		 *
		 * @param[in] updateRate how often the rainbow should be updated
		 */
		RunningSectionsMessage(std::chrono::milliseconds updateRate, uint32_t sectionCount, uint32_t sectionSize)
			: _sectionSize(sectionSize), _sectionCount(sectionCount), _updateRate(updateRate)
		{}

		/**
		 * @brief serializes message to comply with the protocol
		 *
		 * @returns ostringstream shared_ptr containing the serialized message.
		 */
		virtual std::shared_ptr<std::ostringstream> compose() const;

	private:
		/** Variables */
		uint32_t _sectionSize;
		uint32_t _sectionCount;
		std::chrono::milliseconds _updateRate;
	};
}