#pragma once 

#include <string>
#include <atomic>
#include <memory>

#include <QtGui>
#include <QTimer>
#include <QtCore>
#include <QWidget>
#include <QKeyEvent>
#include <QCloseEvent>

#include "ui_MusicQuizGUI.h"

#include "util/QuizSettings.hpp"
#include "media/AudioPlayer.hpp"
#include "media/VideoPlayer.hpp"

#include "gui_tools/widgets/QuizTeam.hpp"
#include "gui_tools/widgets/QuizEntry.hpp"
#include "gui_tools/widgets/QuizBoard.hpp"

namespace common {
	class Configuration;
}

namespace MusicQuiz {
	class QuizFactory;
	class QuizSelector;
	class TeamSelector;
	class QuizIntroScreen;
	class QuizWinningScreen;

	class MusicQuizController : public QWidget
	{
		Q_OBJECT

	public:
		enum QuizState
		{
			SELECT_QUIZ = 0,
			SELECT_TEAM = 1,
			QUIZ_INTRO_SCREEN = 2,
			RUN_QUIZ = 3,
			VICTORY_SCREEN = 4
		};

		/**
		 * @brief Constructor
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit MusicQuizController(const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Destructor
		 */
		virtual ~MusicQuizController();

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		MusicQuizController(const MusicQuizController&) = delete;
		MusicQuizController& operator=(const MusicQuizController&) = delete;

	public slots:

	private slots:
		/**
		 * @brief Handles the quiz states.
		 */
		void executeQuiz();

		/**
		 * @brief Quits the quiz.
		 */
		void quitQuiz();

		/**
		 * @brief Handles quiz selected.
		 *
		 * @param[in] quizIdx The selected quiz index.
		 * @param[in] quizName The selected quiz name.
		 * @param[in] quizAuthor The selected quiz author.
		 * @param[in] settings The quiz settings.
		 */
		void quizSelected(size_t quizIdx, const QString& quizName, const QString& quizAuthor, const MusicQuiz::QuizSettings& settings);

		/**
		 * @brief Handles team selected.
		 *
		 * @param[in] teams The list of teams.
		 */
		void teamSelected(const std::vector<MusicQuiz::QuizTeam*>& teams);

		/**
		 * @brief Handles the intro complete event.
		 */
		void introComplete();

		/**
		 * @brief Handles the quiz complete .
		 */
		void quizCompleted(std::vector<MusicQuiz::QuizTeam*> winningTeam);

	private:

		/** Variables */
		const QString _themeSongFile;
		const QString _victorySongFile;

		MusicQuiz::QuizBoard* _quizBoard = nullptr;
		std::vector< MusicQuiz::QuizTeam* > _teams;
		MusicQuiz::QuizSelector* _quizSelector = nullptr;
		MusicQuiz::TeamSelector* _teamSelector = nullptr;
		MusicQuiz::QuizIntroScreen* _quizIntro = nullptr;
		MusicQuiz::QuizWinningScreen* _quizWinningScreen = nullptr;

		std::vector<MusicQuiz::QuizTeam*> _winningTeams;

		/** Update Timer */
		QTimer _updateTimer;
		const size_t _updateTimerDelayMs = 25;

		/** State Variables */
		std::atomic<bool> _quizSelected;
		std::atomic<bool> _teamSelected;
		std::atomic<bool> _introScreenDone;
		std::atomic<bool> _gameCompleted;
		QuizState _quizState = QuizState::SELECT_QUIZ;

		/** Quiz Settings */
		size_t _selectedQuizIdx = 0;
		QString _quizName = "";
		QString _quizAuthor = "";
		MusicQuiz::QuizSettings _settings;

		/** Audio Player */
		std::shared_ptr<media::AudioPlayer> _audioPlayer = nullptr;

		/** Video Player */
		std::shared_ptr<media::VideoPlayer> _videoPlayer = nullptr;

		const common::Configuration& _config;

	};
}