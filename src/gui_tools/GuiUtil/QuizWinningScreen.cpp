#include "QuizWinningScreen.hpp"

#include <QColor>
#include <QLabel>
#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QSpacerItem>


MusicQuiz::QuizWinningScreen::QuizWinningScreen(const std::vector<MusicQuiz::QuizTeam*>& winningTeams, QWidget* parent) :
	QDialog(parent), _winnerDisplayTime(267000), _winningTeams(winningTeams)
{
	/** Sanity Check */
	if ( _winningTeams.empty() ) {
		screenComplete();
	}

	/** Set Window Flags */
	setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::MSWindowsFixedSizeDialogHint);

	/** Set Object Name */
	setObjectName("QuizWinningScreen");

	/** Create Layout */
	createLayout();

	/** Set Fullscreen */
	showFullScreen();

	/** Set Fixed Size */
	setFixedSize(this->size());

	/** Connect timer */
	connect(&_timer, SIGNAL(timeout()), this, SLOT(screenComplete()));
	connect(&_sizeTimer, SIGNAL(timeout()), this, SLOT(increaseTextSize()));

	/** Start Timer */
	if ( !_timer.isActive() ) {
		_timer.start(_winnerDisplayTime);
	}

	if ( !_sizeTimer.isActive() ) {
		_sizeTimer.start(15);
	}
}

MusicQuiz::QuizWinningScreen::~QuizWinningScreen()
{
	/** Stop Timer */
	if ( !_timer.isActive() ) {
		_timer.stop();
	}
}

void MusicQuiz::QuizWinningScreen::createLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QGridLayout* topLayout = new QGridLayout;
	QGridLayout* bottomLayout = new QGridLayout;
	mainlayout->setRowStretch(0, 1);
	mainlayout->setRowStretch(1, 5);

	/** Text Label */
	QString topLabel = "The Winner!";
	if ( _winningTeams.size() > 1 ) {
		topLabel = "The Winners!";
	}
	QLabel* textLabel = new QLabel(topLabel);
	textLabel->setObjectName("winningScreenTextLabel");
	topLayout->addWidget(textLabel, 0, 0, Qt::AlignCenter);

	/** Winners Label */
	QLabel* label = new QLabel(_winningTeams[0]->getName());
	bottomLayout->addWidget(label, 0, 0, Qt::AlignCenter);
	_winnersLabels.push_back(label);

	for ( int i = 0; i < static_cast<int>(_winningTeams.size()) - 1; ++i ) {
		/** & Label */
		label = new QLabel("&");
		bottomLayout->addWidget(label, 0, i * 2 + 1, Qt::AlignCenter);
		_andLabels.push_back(label);

		/** Team Name Label */
		label = new QLabel(_winningTeams[i + 1]->getName());
		bottomLayout->addWidget(label, 0, i * 2 + 2, Qt::AlignCenter);
		_winnersLabels.push_back(label);
	}

	/** Add Layouts */
	mainlayout->addItem(topLayout, 0, 0);
	mainlayout->addItem(bottomLayout, 1, 0);

	/** Set Layout */
	setLayout(mainlayout);
	setContentsMargins(20, 20, 20, 20);
}

void MusicQuiz::QuizWinningScreen::increaseTextSize()
{
	/** Sanity Check */
	if ( _winnersLabels.empty() ) {
		return;
	}

	/** Get Text Width */
	size_t width = 0;
	for ( size_t i = 0; i < _winnersLabels.size(); ++i ) {
		width += _winnersLabels[i]->fontMetrics().boundingRect(_winnersLabels[i]->text()).width();
	}

	for ( size_t i = 0; i < _andLabels.size(); ++i ) {
		width += _andLabels[i]->fontMetrics().boundingRect(_andLabels[i]->text()).width() + 20;
	}

	/** Increase Size */
	const size_t screenWidth = size().width();
	if ( width < screenWidth - 200 ) {
		++_textSize;
	}

	/** Choose Random Color */
	_textColor.setHsv(_hueCounter, _textColor.saturation(), _textColor.value());
	_hueCounter += 1;
	if ( _hueCounter > 210 && _hueCounter < 260 ) { // Skip Blue
		_hueCounter = 270;
	} else if ( _hueCounter >= 359 ) { // Loop Color
		_hueCounter = 0;
	}

	/** Set Stylesheet */
	if ( !_andLabels.empty() ) {
		for ( size_t i = 0; i < _andLabels.size(); ++i ) {
			_andLabels[i]->setStyleSheet("font-size: " + QString::number((_textSize > 600 ? 150 : _textSize / 4)) + "px; color: yellow;");
		}
	}

	for ( size_t i = 0; i < _winnersLabels.size(); ++i ) {
		_winnersLabels[i]->setStyleSheet("font-size: " + QString::number(_textSize / 4) + "px; font-weight: bold;" +
			"color: rgb(" + QString::number(_textColor.red()) + "," + QString::number(_textColor.green()) + "," + QString::number(_textColor.blue()) + ");");
	}
}

void MusicQuiz::QuizWinningScreen::screenComplete()
{
	emit winningScreenCompleteSignal();
}