#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <QString>

#include "media/AudioPlayer.hpp"
#include "media/VideoPlayer.hpp"
#include "gui_tools/widgets/QuizCategory.hpp"

namespace common {
	class Configuration;
}


namespace MusicQuiz {
	namespace util {
		class QuizLoader
		{
		public:
			struct QuizPreview
			{
				std::string quizName = "";
				std::string quizAuthor = "";
				bool includeSongs = false;
				bool includeVideos = false;
				bool guessTheCategory = false;
				std::string quizDescription = "";
				std::vector<std::string> categories;
				std::vector<std::string> rowCategories;

				friend std::ostream& operator<<(std::ostream& out, const QuizPreview& quizPreview)
				{
					out << "\n\nQuiz Name: " << quizPreview.quizName << "\n";
					out << "Quiz Author: " << quizPreview.quizAuthor << "\n";
					out << "Quiz Description: " << quizPreview.quizDescription << "\n";
					if ( !quizPreview.categories.empty() ) {
						out << "Quiz Categories:\n";
						for ( size_t i = 0; i < quizPreview.categories.size(); ++i ) {
							out << "\t" << quizPreview.categories[i] << "\n";
						}
					}

					if ( !quizPreview.rowCategories.empty() ) {
						out << "Quiz Row Categories:\n";
						for ( size_t i = 0; i < quizPreview.rowCategories.size(); ++i ) {
							out << "\t" << quizPreview.rowCategories[i] << "\n";
						}
					}

					out << "Quiz Include Songs: " << (quizPreview.includeSongs ? "Yes" : "No") << "\n";
					out << "Quiz Include Videos: " << (quizPreview.includeVideos ? "Yes" : "No") << "\n";
					out << "Quiz Quess the Category: " << (quizPreview.guessTheCategory ? "Yes" : "No") << "\n\n";
					return out;
				}
			};

			/**
			 * @brief Deleted constructor.
			 */
			QuizLoader() = delete;

			/**
			* @brief Deleted Destructor.
			*/
			~QuizLoader() = delete;

			/**
			 * @brief Deleted the copy and assignment constructor.
			 */
			QuizLoader(const QuizLoader&) = delete;
			QuizLoader& operator=(const QuizLoader&) = delete;

			/**
			* @brief Returns a list of quizzez stored in the data folder.
			*
			* @return The list of quizzes.
			*/
			static std::vector<std::string> getListOfQuizzes(const common::Configuration& config);

			/**
			* @brief Returns a quiz preview.
			*
			* @param[in] idx The index of the quiz to preview.
			*
			* @return The quiz preview.
			*/
			static QuizPreview getQuizPreview(size_t idx, const common::Configuration& config);

			/**
			* @brief Returns a list of the categories.
			*
			* @param[in] idx The index of the quiz to load the categories from.
			* @param[out] err The error message.
			*
			* @return The quiz categories.
			*/
			static std::vector<MusicQuiz::QuizCategory*> loadQuizCategories(size_t idx, const std::shared_ptr< media::AudioPlayer >& audioPlayer,
				const std::shared_ptr< media::VideoPlayer >& videoPlayer, const common::Configuration& config, std::string& err);

			/**
			* @brief Returns a list of the row categories.
			*
			* @param[in] idx The index of the quiz to load the categories from.
			*
			* @return The quiz row categories.
			*/
			static std::vector<QString> loadQuizRowCategories(size_t idx, const common::Configuration& config);


		protected:
			/** Variables */
		};
	}
}