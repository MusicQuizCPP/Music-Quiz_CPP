#pragma once

#include <string>

namespace common {
	/**
	 * Configuration object
	 */
	class Configuration
	{
	public:		
		/**
		 * @brief Constructor
		 */
		Configuration();

		/**
		 * @brief Destructor
		 */
		~Configuration() {}

		/**
		 * @brief Get the configured path to the quiz data.
		 *
		 * @return quiz data path.
		 */
		std::string getQuizDataPath() const;

        /**
         * @brief Set the quiz data path.
		 * 
		 * @param[in] path the new path.
         */
        void setQuizDataPath(std::string path);

		/**
		 * @brief Check if quiz data path exist.
		 * 
		 * @return does quiz data path exist.
		 */
        bool doQuizDataPathExist() const;

		/**
		 * @brief Check if quiz the quiz config file exist.
		 * 
		 * @return True if the quiz config file exists.
		 */
        bool doQuizConfigFileExist() const;

		/**
		 * @brief Loads the config file and sets the _quizDataPath.
		 */
        void loadConfigurationFile();

		/**
		 * @brief Saves the config file.
		 */
		void saveConfigurationFile();

		/**
		 * @brief Convert media path to full path.
		 * 
		 * @param[in] mediaPathStr the path to be converted.
		 * 
		 * @return converted path.
		 */
		std::string mediaPathToFullPath(std::string mediaPathStr) const;

    private:
		/** The quiz data path. */
        std::string _quizDataPath;

		/** The quiz data path. */
		const std::string _configFilePath = "./config.xml";
	};
}