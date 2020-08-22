#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QAbstractButton>
#include <QFileDialog>

#include "common/Log.hpp"
#include "common/Configuration.hpp"

#include "MusicQuizController.hpp"
#include "gui_tools/QuizCreator/QuizCreator.hpp"
#include "youtube_wrapper.h"

static void errorMessage(const std::string& title, const std::string& errorMsg);
static int runQuizCreator(QApplication& app, const common::Configuration& config);
static int runMusicQuiz(QApplication& app, const common::Configuration& config);
static void selectQuizData(common::Configuration& config);

static void errorMessage(const std::string& title, const std::string& errorMsg)
{
	QMessageBox::critical(nullptr, QString::fromLocal8Bit(title.c_str()), QString::fromLocal8Bit(errorMsg.c_str()), QMessageBox::Ok);
}

static int runMusicQuiz(QApplication& app, const common::Configuration& config)
{
	LOG_INFO("Music Quiz Selected.");

	try {
		MusicQuiz::MusicQuizController w(config);
		app.exec();
	} catch ( const std::exception& err ) {
		LOG_ERROR("Failed to start Music Quiz. " << err.what());
		errorMessage("Quiz Creator", "Quiz Creator Error. " + std::string(err.what()));
		return 1;
	} catch ( ... ) {
		LOG_ERROR("Failed to start Music Quiz. Unknown Error.");
		errorMessage("Quiz Creator", "Quiz Creator Error. Unknown Error.");
		return 1;
	}
	return 0;
}

static int runQuizCreator(QApplication& app, const common::Configuration& config)
{
	LOG_INFO("Quiz Creator Selected.");

	try {
		MusicQuiz::QuizCreator w(config);
		w.show();
		app.exec();
	} catch ( const std::exception& err ) {
		LOG_ERROR("Failed to start Quiz Creator. " << err.what());
		errorMessage("Quiz Creator", "Quiz Creator Error. " + std::string(err.what()));
		return 1;
	} catch ( ... ) {
		LOG_ERROR("Failed to start Quiz Creator. Unknown Error.");
		errorMessage("Quiz Creator", "Quiz Creator Error. Unknown Error.");
		return 1;
	}
	return 0;
}

static void selectQuizData(common::Configuration& config)
{
	/** Open File Dialog */
	const QString path = QFileDialog::getExistingDirectory(nullptr, "Select Quiz Data Folder", QString::fromStdString(config.getQuizDataPath()));
	if ( path.isEmpty() ) {
		return;
	}
	config.setQuizDataPath(path.toStdString());
	LOG_INFO("Set quiz path to " + path.toStdString());
}

int main(int argc, char* argv[])
{
	GoString video;
	video.p = "https://www.youtube.com/watch?v=LXb3EKWsInQ";
	video.n = strlen(video.p);

	LOG_INFO(std::string(getVideoUrl(video)));

	/** Create QApplication */
	QApplication app(argc, argv);

	common::Configuration config;

	/** Set Stylesheet */
	QFile qss(QString::fromStdString(":/stylesheet_musicQuiz.qss"));
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	/** Pop to select program (MusicQuiz or QuizCreator) */
	QMessageBox msgBox(QMessageBox::Question, "Select Program", "Select Program", QMessageBox::NoButton, nullptr, Qt::WindowStaysOnTopHint);
	QAbstractButton* musicQuizButton = msgBox.addButton("Music Quiz", QMessageBox::YesRole);
	QAbstractButton* quizCreatorButton = msgBox.addButton("Quiz Creator", QMessageBox::YesRole);
	QAbstractButton* selectQuizDataButton = msgBox.addButton("Select Quiz Directory", QMessageBox::YesRole);
	QAbstractButton* exitButton = msgBox.addButton("Exit", QMessageBox::NoRole);

	msgBox.setStyleSheet(qss.readAll());
	qss.close();

	while ( !config.doQuizDataPathExist() ) {
		selectQuizData(config);
	}

	while ( true ) {
		msgBox.exec();
		/** Start Selected Program */
		if ( msgBox.clickedButton() == musicQuizButton ) {
			return runMusicQuiz(app, config);
		} else if ( msgBox.clickedButton() == quizCreatorButton ) {
			return runQuizCreator(app, config);
		} else if ( msgBox.clickedButton() == selectQuizDataButton ) {
			selectQuizData(config);
		} else if ( msgBox.clickedButton() == exitButton ) {
			/** Close Program */
			LOG_INFO("Exit Program Selected.");
			return 0;
		}
	}
	return 0;
}