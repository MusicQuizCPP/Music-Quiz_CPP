#include "QuizIntroScreen.hpp"

#include <QColor>
#include <QLabel>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>


MusicQuiz::QuizIntroScreen::QuizIntroScreen(const QString& quizName, const QString& quizAuthor, QWidget* parent) :
	QDialog(parent), _quizName(quizName), _quizAuthor(quizAuthor), _introDisplayTime(4000)
{
	/** Set Window Flags */
	setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);

	/** Set Object Name */
	setObjectName("QuizIntroScreen");

	/** Create Layout */
	createLayout();

	/** Set Fullscreen */
	showFullScreen();

	/** Connect timer */
	connect(&_timer, SIGNAL(timeout()), this, SLOT(introComplete()));

	/** Start Timer */
	if ( !_timer.isActive() ) {
		_timer.start(_introDisplayTime);
	}
}

MusicQuiz::QuizIntroScreen::~QuizIntroScreen()
{
	/** Stop Timer */
	if ( !_timer.isActive() ) {
		_timer.stop();
	}
}

void MusicQuiz::QuizIntroScreen::createLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	mainlayout->setRowStretch(0, 1);
	mainlayout->setRowStretch(1, 3);
	mainlayout->setRowStretch(2, 1);
	mainlayout->setRowStretch(3, 1);
	mainlayout->setRowStretch(4, 3);

	/** Quiz Label */
	QLabel* label = new QLabel("Music Quiz");
	label->setObjectName("quizIntroLabel");
	mainlayout->addWidget(label, 1, 0, Qt::AlignCenter);

	/** Quiz Name */
	QVBoxLayout* nameLayout = new QVBoxLayout;
	label = new QLabel(_quizName);
	label->setObjectName("quizIntroNameLabel");
	label->setAlignment(Qt::AlignCenter);
	nameLayout->addWidget(label);

	/** Quiz Author */
	if ( !_quizAuthor.isEmpty() ) {
		label = new QLabel("By " + _quizAuthor);
		label->setObjectName("quizIntroAuthorLabel");
		label->setAlignment(Qt::AlignCenter);
		nameLayout->addWidget(label);
		mainlayout->addItem(nameLayout, 3, 0);
	}

	/** Set Layout */
	setLayout(mainlayout);
	setContentsMargins(20, 20, 20, 20);
}

void MusicQuiz::QuizIntroScreen::introComplete()
{
	emit introCompleteSignal();
}