#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QApplication>
#include <QTextToSpeech>
#include <QAbstractButton>

#include "common/Log.hpp"
#include "common/Configuration.hpp"

#include "MusicQuizController.hpp"
#include "gui_tools/QuizCreator/QuizCreator.hpp"

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
	QAbstractButton* selectQuizDataButton = msgBox.addButton("", QMessageBox::YesRole);
	QPixmap pixmap(":imgs/browse_file_icon.png");
	QIcon icon(pixmap);
	selectQuizDataButton->setIcon(icon);
	QAbstractButton* exitButton = msgBox.addButton("Exit", QMessageBox::NoRole);

	msgBox.setStyleSheet(qss.readAll());
	qss.close();

	if ( !config.doQuizDataPathExist() ) {
		/** Popup to tell the user to select a data folder */
		QMessageBox::information(nullptr, "Select Data Directory", "No Data Directory Found!\n\nSelect a directory containing the quiz data. This is required to load existing- and save new quizzes.", QMessageBox::Ok);

		/** Popup to select the data folder */
		selectQuizData(config);

		/** Check if data was set */
		if ( !config.doQuizDataPathExist() ) {
			QMessageBox::information(nullptr, "Information", "No directory selected! Closing application.", QMessageBox::Ok);

			/** Close Program */
			LOG_INFO("Closing program. No data directory selected.");
			return 0;
		}
	}

	// List the available engines.
	QStringList engines = QTextToSpeech::availableEngines();
	qDebug() << "Available engines:";
	for ( auto engine : engines ) {
		qDebug() << "  " << engine;
	}
	// Create an instance using the default engine/plugin.
	QTextToSpeech* speech = new QTextToSpeech();
	// List the available locales.
	qDebug() << "Available locales:";
	for ( auto locale : speech->availableLocales() ) {
		qDebug() << "  " << locale;
	}
	// Set locale.
	speech->setLocale(QLocale(QLocale::English, QLocale::LatinScript, QLocale::UnitedStates));
	// List the available voices.
	qDebug() << "Available voices:";
	for ( auto voice : speech->availableVoices() ) {
		qDebug() << "  " << voice.name();
	}
	// Display properties.
	qDebug() << "Locale:" << speech->locale();
	qDebug() << "Pitch:" << speech->pitch();
	qDebug() << "Rate:" << speech->rate();
	qDebug() << "Voice:" << speech->voice().name();
	qDebug() << "Volume:" << speech->volume();
	qDebug() << "State:" << speech->state();
	// Say something.
	speech->say("Hello, Stefan! QT Has a speech engine build in. What The Fuck. This if awesome. Now we can create speech songs!!!");

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