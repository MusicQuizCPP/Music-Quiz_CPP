#pragma once

#include <chrono>
#include <iostream>


namespace LightControl {
	class PendingMessage {
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] data ostringstream buffer containing the message.
		 * @param[in] timeout how long the message is allowed to be queued before it expires
		 */
		PendingMessage(std::shared_ptr<std::ostringstream> data, std::chrono::milliseconds timeout) :
			_data(data),
			_queueTime(std::chrono::system_clock::now()),
			_timeout(std::chrono::duration_cast<std::chrono::milliseconds>(timeout))
		{}

		/**
		 * @brief evaluates if the message is timed out
		 *
		 * @returns is the message timed out
		 */
		bool isTimedOut()
		{
			return std::chrono::system_clock::now() > _queueTime + _timeout;
		}

		/** Variables */
		std::shared_ptr<std::ostringstream> _data;

	private:
		/** Variables */
		std::chrono::milliseconds _timeout;
		std::chrono::time_point<std::chrono::system_clock> _queueTime;
	};
}