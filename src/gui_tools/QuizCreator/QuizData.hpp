#pragma once

#include <QString>
#include <vector>

#include "common/Configuration.hpp"
#include "media/AudioPlayer.hpp"
#include "media/VideoPlayer.hpp"

#include <boost/property_tree/ptree.hpp>
#include <string>

namespace MusicQuiz {
    class CategoryCreator;

    class QuizData {
        public:
            /**
             * @brief Constructor create empty quizdata
             *
             * @param[in] config configuration.
             */
            QuizData(const common::Configuration& config) : _config(config) {}

            /**
             * @brief Constructor create quizdata from existing quiz on disk
             *
             * @param[in] name The quiz name.
             * @param[in] audioPlayer The audio player.
             * @param[in] skipEntries skip loading entries.
             * @param[in] config configuration.
             * @param[in] parent The parent widget.
             */
            QuizData(const common::Configuration& config, const std::string& name, const media::AudioPlayer::Ptr& audioPlayer, 
                bool skipEntries = false, QWidget* parent = nullptr);
            
            /**
             * @brief Save quiz to disk
             *
             */
            void save() const;
            
            /**
             * @brief Sets the author
             * 
             * @param[in] author author to set
             */
            void setAuthor(const std::string& author) {_author = author;}

            /**
             * @brief Get the author
             * 
             * @return The author
             */
            std::string getAuthor() const {return _author;}

            /**
             * @brief Sets the quiz name
             * 
             * @param[in] name quiz name to set
             */
            void setName(const std::string& name) {_name = name;}

            /**
             * @brief Get the quiz name
             * 
             * @return The quiz name
             */

            std::string getName() const {return _name;}

            /**
             * @brief Sets the quiz description
             * 
             * @param[in] description description to set
             */
            void setDescription(const std::string& description) {_description = description;}

            /**
             * @brief Get the quiz description
             * 
             * @return The quiz description
             */
            std::string getDescription() const {return _description;}

            /**
             * @brief Sets if guess the category should be enabled
             * 
             * @param[in] enabled whether guess the category should be enabled
             * @param[in] points How many points guess the category should give
             */
            void setGuessTheCategory(bool enabled, size_t points = 0) { 
                _guessTheCategory = enabled;
                _guessTheCategoryPoints = points;
            }

            /**
             * @brief Check whether guess the category is enabled
             * 
             * @return is guess the category enabled
             */
            bool getGuessTheCategory() const {return _guessTheCategory;}

            /**
             * @brief Get the points for guess the category
             * 
             * @return How many points guess the category will give
             */
            size_t getGuessTheCategoryPoints() const {return _guessTheCategoryPoints;}

            /**
             * @brief Sets the categories
             * 
             * @param[in] categories categories to set
             */
            void setCategories(std::vector< MusicQuiz::CategoryCreator* > categories) {_categories = categories;}

            /**
             * @brief Get the categories
             * 
             * @return The categories
             */
            const std::vector< MusicQuiz::CategoryCreator* >& getCategories() const {return _categories;}

            /**
             * @brief Get single category
             * @param[in] categoryName category to get
             * 
             * @return The category. nullptr if not found.
             */

            MusicQuiz::CategoryCreator* getCategory(const std::string& categoryName) const;

            /**
             * @brief Sets the row categories
             * 
             * @param[in] rowCategories row categories to set
             */

            void setRowCategories(std::vector < std::string > rowCategories);

            /**
             * @brief Sets the row categories
             * 
             * @param[in] rowCategories row categories to set
             */
            void setRowCategories(std::vector < QString > rowCategories);

            /**
             * @brief Get the row categories
             * 
             * @return The row categories
             */
            const std::vector<std::string>& getRowCategories() const {return _rowCategories;}

            /**
             * @brief Check whether the intended save path already exists
             * 
             * @return if the intended save path exists
             */
            bool doesQuizDirectoryExist() const;

            /**
             * @brief Get quiz directory path
             * 
             * @return path to quiz directory
             */
            std::string getQuizPath() const;
            
            /**
             * @brief Get quiz media path
             * 
             * @return path to media directory
             */
            std::string getMediaPath() const;

        private:
            const common::Configuration& _config;

            std::string _name = "";
            std::string _author = "";
            std::string _description = "";

            bool _guessTheCategory = false;
            size_t _guessTheCategoryPoints = 0;

            std::vector< std::string > _rowCategories;
            std::vector< MusicQuiz::CategoryCreator* > _categories;

            /**
             * @brief Check if category names are unique
             * 
             * @return whether category names are unique
             */
            bool areCategoryNamesUnique() const;

            /**
             * @brief Create ptree containing the serialised quiz data
             * 
             * @param[in] savePath where to save media files during ptree construction
             * 
             * @return Generated ptree
             */
            boost::property_tree::ptree constructPtree(const std::string& savePath) const;

            /**
             * @brief Save cheat sheet
             * 
             * @param[in] path path where the cheat sheet should be saved
             * 
             * @return generated ptree
             */
            void saveCheatSheet(const std::string &path) const;

            /**
             * @brief Create the quiz directory
             * 
             * @return generated ptree
             */
            void createQuizDirectory() const;

            /**
             * @brief Create a directory
             * @param[in] path path of the directory to create
             * @param[in] errorString string containing what should be given as error
             *            if the directory could not be created
             */
            void createDirectory(const std::string& path, const std::string& errorString) const;

            /**
             * @brief Load categories from ptree
             * @param[in] tree tree to load the categories from
             * @param[in] audioPlayer Player the entries should use to play audio
             * @param[in] skipEntries whether to skip loading entries
             * @param[in] parent widget to set as parent
             */
            std::vector< MusicQuiz::CategoryCreator* > loadCategories(boost::property_tree::ptree &tree, const media::AudioPlayer::Ptr& audioPlayer, 
                bool skipEntries, QWidget* parent) const;    
    };
}

