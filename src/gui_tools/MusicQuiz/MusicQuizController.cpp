#include "MusicQuizController.hpp"

#include <thread>
#include <string>
#include <chrono>

#include <QRect>
#include <QMessageBox>
#include <QApplication>

#include "common/Log.hpp"
#include "common/Configuration.hpp"

#include "gui_tools/widgets/QuizFactory.hpp"
#include "gui_tools/widgets/QuizCategory.hpp"
#include "gui_tools/GuiUtil/QuizSelector.hpp"
#include "gui_tools/GuiUtil/TeamSelector.hpp"
#include "gui_tools/GuiUtil/QuizIntroScreen.hpp"
#include "gui_tools/GuiUtil/QuizWinningScreen.hpp"


MusicQuiz::MusicQuizController::MusicQuizController(const common::Configuration& config, QWidget* parent) :
	QWidget(parent),
	_themeSongFile((config.getQuizDataPath() + "/default/theme_song.mp3").c_str()),
	_victorySongFile((config.getQuizDataPath() + "/default/victory_song.mp3").c_str()),
	_updateTimerDelayMs(25), _quizSelected(false), _teamSelected(false),
	_introScreenDone(false), _gameCompleted(false), _config(config)
{
	/** Create Audio Player */
	_audioPlayer = std::make_shared<media::AudioPlayer>();

	/** Create Video Player */
	_videoPlayer = std::make_shared<media::VideoPlayer>();
	_videoPlayer->setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);

	/** Set Video Player Size */
	const QRect screenRec = QGuiApplication::primaryScreen()->geometry();
	_videoPlayer->setMinimumSize(QSize(screenRec.width(), screenRec.height()));
	_videoPlayer->resize(QSize(screenRec.width(), screenRec.height()));

	/** Center Video Player */
	_videoPlayer->move(0, 0);

	/** Connect Update Timer */
	connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(executeQuiz()));

	/** Start Timer */
	_updateTimer.start(_updateTimerDelayMs);
}

MusicQuiz::MusicQuizController::~MusicQuizController()
{
	/** Stop Audio */
	if ( _audioPlayer != nullptr ) {
		_audioPlayer->stop();
	}

	/** Stop Video */
	if ( _audioPlayer != nullptr ) {
		_videoPlayer->stop();
		_videoPlayer->hide();
	}

	/** Stop Update Timer */
	if ( _updateTimer.isActive() ) {
		_updateTimer.stop();
	}

	/** Close Quiz Selector */
	if ( _quizSelector != nullptr ) {
		_quizSelector->close();
		_quizSelector = nullptr;
	}

	/** Close Team Selector */
	if ( _teamSelector != nullptr ) {
		_teamSelector->close();
		_teamSelector = nullptr;
	}

	/** Close Quiz Board */
	if ( _quizBoard != nullptr ) {
		_quizBoard = nullptr;
		delete _quizBoard;
	}

	/** Close Quiz Intro */
	if ( _quizIntro != nullptr ) {
		_quizIntro->close();
		_quizIntro = nullptr;
	}

	/** Close Quiz Winning Screen */
	if ( _quizWinningScreen != nullptr ) {
		_quizWinningScreen->close();
		_quizWinningScreen = nullptr;
	}
}

void MusicQuiz::MusicQuizController::executeQuiz()
{
	switch ( _quizState ) {
	case MusicQuiz::MusicQuizController::SELECT_QUIZ:
	{
		/** Start Quiz Theme Song */
		_audioPlayer->play(_themeSongFile, true);

		/** Create Quiz Selector */
		_quizSelector = new MusicQuiz::QuizSelector(_config);

		/** Connect Signals */
		connect(_quizSelector, SIGNAL(quitSignal()), this, SLOT(quitQuiz()));
		connect(_quizSelector, SIGNAL(quizSelectedSignal(size_t, const QString&, const QString&, const MusicQuiz::QuizSettings&)), this, SLOT(quizSelected(size_t, const QString&, const QString&, const MusicQuiz::QuizSettings&)));

		/** Show widget */
		_quizSelector->exec();

		/** Go to select team state */
		_quizState = SELECT_TEAM;
	}
	break;
	case MusicQuiz::MusicQuizController::SELECT_TEAM:
	{
		/** Wait for quiz selection to complete */
		if ( !_quizSelected ) {
			break;
		}

		/** Create Team Selector */
		_teamSelector = new MusicQuiz::TeamSelector;

		/** Connect Signals */
		connect(_teamSelector, SIGNAL(quitSignal()), this, SLOT(quitQuiz()));
		connect(_teamSelector, SIGNAL(teamSelectedSignal(const std::vector<MusicQuiz::QuizTeam*>&)), this, SLOT(teamSelected(const std::vector<MusicQuiz::QuizTeam*>&)));

		/** Show Widget */
		_teamSelector->exec();

		/** Go to select quiz intro screen state */
		_quizState = QUIZ_INTRO_SCREEN;
	}
	break;
	case MusicQuiz::MusicQuizController::QUIZ_INTRO_SCREEN:
	{
		/** Wait for team selection to complete */
		if ( !_teamSelected ) {
			break;
		}

		/** Show Intro Screen */
		_quizIntro = new MusicQuiz::QuizIntroScreen(_quizName, _quizAuthor, this);

		/** Connect Signals */
		try {
			connect(_quizIntro, SIGNAL(introCompleteSignal()), this, SLOT(introComplete()));
		} catch ( const std::exception& err ) {
			QMessageBox::warning(nullptr, "Failed to Display Quiz Intro", "Failed to display quiz intro. " + QString(err.what()));
			_quizState = SELECT_QUIZ;
			break;
		} catch ( ... ) {
			QMessageBox::warning(nullptr, "Failed to Quiz Intro", "Failed to display quiz intro.");
			_quizState = SELECT_QUIZ;
			break;
		}

		/** Show Widget */
		_quizIntro->exec();

		/** Go to select team state */
		_quizState = RUN_QUIZ;
	}
	break;
	case MusicQuiz::MusicQuizController::RUN_QUIZ:
	{
		/** Wait for intro scree to be done showing */
		if ( !_introScreenDone ) {
			break;
		}

		try {
			/** Create Quiz Board */
			_quizBoard = MusicQuiz::QuizFactory::createQuiz(_selectedQuizIdx, _settings, _audioPlayer, _videoPlayer, _config, _teams);

			/** Connect Signals */
			connect(_quizBoard, SIGNAL(quitSignal()), this, SLOT(quitQuiz()));
			connect(_quizBoard, SIGNAL(gameComplete(std::vector<MusicQuiz::QuizTeam*>)), this, SLOT(quizCompleted(std::vector<MusicQuiz::QuizTeam*>)));

			/** Stop Quiz Theme Song */
			_audioPlayer->stop();

			/** Show Widget */
			_quizBoard->exec();

			/** Go to quiz running state */
			_quizState = VICTORY_SCREEN;
		} catch ( const std::exception& err ) {
			QMessageBox::warning(nullptr, "Failed to Load Quiz", "Failed to load the quiz. " + QString(err.what()));
			_quizState = SELECT_QUIZ;
			break;
		} catch ( ... ) {
			QMessageBox::warning(nullptr, "Failed to Load Quiz", "Failed to load the quiz.");
			_quizState = SELECT_QUIZ;
			break;
		}
	}
	break;
	case MusicQuiz::MusicQuizController::VICTORY_SCREEN:
	{
		/** Wait for Quiz to complete */
		if ( !_gameCompleted ) {
			break;
		}

		/** Show Victory Screen */
		if ( !_winningTeams.empty() ) {
			/** Create Winning Screen */
			_quizWinningScreen = new QuizWinningScreen(_winningTeams);

			/** Connect Signals */
			connect(_quizWinningScreen, SIGNAL(winningScreenCompleteSignal()), this, SLOT(quitQuiz()));

			/** Start Winning Song */
			_audioPlayer->play(_victorySongFile, true);

			/** Show Widget */
			_quizWinningScreen->exec();
		}

		/** Go to victory screen state */
		_quizState = VICTORY_SCREEN;
	}
	break;
	default:
		break;
	}
}

void MusicQuiz::MusicQuizController::quitQuiz()
{
	/** Call Destructor */
	QApplication::quit();
}

void MusicQuiz::MusicQuizController::quizSelected(const size_t quizIdx, const QString& quizName, const QString& quizAuthor, const MusicQuiz::QuizSettings& settings)
{
	/** Set Quiz Selected */
	_quizSelected = true;
	_selectedQuizIdx = quizIdx;
	_quizName = quizName;
	_quizAuthor = quizAuthor;

	/** Set Settings */
	_settings = settings;

	/** Remove Quiz Selector */
	_quizSelector->hide();
	delete _quizSelector;
	_quizSelector = nullptr;
}

void MusicQuiz::MusicQuizController::teamSelected(const std::vector<MusicQuiz::QuizTeam*>& teams)
{
	/** Set Quiz Selected */
	_teamSelected = true;

	/** Set Teams */
	_teams = teams;

	/** Remove Team Selector */
	_teamSelector->hide();
	delete _teamSelector;
	_teamSelector = nullptr;
}

void MusicQuiz::MusicQuizController::introComplete()
{
	/** Set Intro Done */
	_introScreenDone = true;

	/** Remove Intro Screen */
	_quizIntro->hide();
	delete _quizIntro;
	_quizIntro = nullptr;
}

void MusicQuiz::MusicQuizController::quizCompleted(std::vector<MusicQuiz::QuizTeam*> winningTeam)
{
	/** Set Game Done */
	_gameCompleted = true;

	/** Set Winning Teams */
	_winningTeams = winningTeam;

	/** Hide Quiz Board */
	_quizBoard->hide();
}