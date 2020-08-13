#pragma once

#include <cstddef>
#include <string>
namespace MusicQuiz {
	struct QuizSettings {
		/** Daily Double */
		bool dailyDouble = false;
		bool dailyDoubleHidden = true;
		int dailyDoublePercentage = 15;

		/** Daily Trible */
		bool dailyTriple = false;
		bool dailyTripleHidden = true;
		int dailyTriplePercentage = 5;

		/** Teams */
		bool hiddenTeamScore = false;

		/** Answers */
		bool hiddenAnswers = false;

		/** Guess the category */
		bool guessTheCategory = false;
		size_t pointsPerCategory = 500;

		/** Light Device */
		std::string deviceIP;
	};
}