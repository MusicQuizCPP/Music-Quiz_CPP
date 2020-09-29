#pragma once

#include <string>

namespace common {
	/**
	 * Configuration object
	 */
	class Configuration
	{
	public:
		Configuration();

		~Configuration() {}

		/**
		 * @brief Get the configured path to the quiz data
		 *
		 * @return quiz data path
		 */
		std::string getQuizDataPath() const;

        /**
         * @brief Set the quiz data path
		 * 
		 * @param[in] path the new path
         */
        void setQuizDataPath(std::string path);

		/**
		 * @brief Check if quiz data path exist
		 * 
		 * @return does quiz data path exist
		 */
        bool doQuizDataPathExist() const;

		/**
		 * @brief Convert media path to full path
		 * 
		 * @param[in] mediaPathStr the path to be converted
		 * 
		 * @return converted path
		 */
		std::string mediaPathToFullPath(std::string mediaPathStr) const;

    private:
        std::string _quizDataPath;
	};
}