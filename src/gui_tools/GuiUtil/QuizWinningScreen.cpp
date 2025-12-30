#include "QuizWinningScreen.hpp"

#include <cmath>
#include <algorithm>

#include <QColor>
#include <QLabel>
#include <QWidget>
#include <QString>
#include <QPainter>
#include <QGridLayout>
#include <QSpacerItem>
#include <QPainterPath>
#include <QFontDatabase>
#include <QRandomGenerator>

#include "util/FontUtil.hpp"


MusicQuiz::QuizWinningScreen::QuizWinningScreen(const std::vector<MusicQuiz::QuizTeam*>& winningTeams, QWidget* parent) :
	QDialog(parent), _winnerDisplayTime(300000), _winningTeams(winningTeams)
{
	/** Sanity Check */
	if ( _winningTeams.empty() ) {
		screenComplete();
	}

	/** Set Window Flags */
	setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::MSWindowsFixedSizeDialogHint);

	/** Set Fullscreen */
	showFullScreen();

	/** Set Fixed Size */
	setFixedSize(this->size());
	
	/** Set font */
	const int fontIndex = QFontDatabase::addApplicationFont(":/fonts/BebasNeue-Regular.ttf");
	const QString fontFamily = QFontDatabase::applicationFontFamilies(fontIndex).at(0);

	QFont font(fontFamily);
	font.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
	setFont(font);

	/** Get Winner Label Text */
	_winnerLabel = "The Winner!";
	if ( _winningTeams.size() > 1 ) {
		_winnerLabel = "The Winners!";
	}

	/** Get Winning Team Names */
	_winnerNames = _winningTeams[0]->getName();
	for ( int i = 0; i < static_cast<int>(_winningTeams.size()) - 1; ++i ) {
		_winnerNames += " & " + _winningTeams[i + 1]->getName();
	}

	/** Connect timeout timer */
	connect(&_timeOutTimer, SIGNAL(timeout()), this, SLOT(screenComplete()));

	/** Start timeout Timer */
	if ( !_timeOutTimer.isActive() ) {
		_timeOutTimer.start(_winnerDisplayTime);
	}

	/** Connect animation timer */
	connect(&_animationTimer, &QTimer::timeout, this, [this]() {
		_animationPhase += 0.01;
		_particles.update();
		_confetti.update();
		update();
	});

	/** Start animation Timer */
	_animationTimer.start(16); // ~60 FPS

	/** Start Confetti Animation */
	_confetti.start(rect(), 20, 1000, 3000);
}

MusicQuiz::QuizWinningScreen::~QuizWinningScreen()
{
	/** Stop Timers */
	if ( !_timeOutTimer.isActive() ) {
		_timeOutTimer.stop();
	}

	if ( !_animationTimer.isActive() ) {
		_animationTimer.stop();
	}
}

void MusicQuiz::QuizWinningScreen::screenComplete()
{
	emit winningScreenCompleteSignal();
}

void MusicQuiz::QuizWinningScreen::paintEvent(QPaintEvent* event)
{
	/** Setup Painter */
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	/** Get Rect */
	QRect r = rect();

	/** Draw Background */
	p.fillRect(r, _backgroundColor);

	/** Draw Particles & Confetti */
	_particles.draw(p);
	_confetti.draw(p);

	/** Create Fitted Font */
	p.setRenderHint(QPainter::TextAntialiasing);
	p.setFont(_winnerLabelFittedFont);

	/** Draw Text Shadow */
	QPoint shadowOffset(4, 4);
	QColor shadowColor(0, 0, 0, 150);
	p.setPen(shadowColor);
	p.drawText(_winnerLabelRect.translated(shadowOffset), Qt::AlignCenter, _winnerLabel);

	/** Draw Winner Text */
	const QPointF start(_winnerLabelRect.center().x(), _winnerLabelRect.top());
	const QPointF end(_winnerLabelRect.center().x(), _winnerLabelRect.bottom());
	QLinearGradient gradient(start, end);
	gradient.setColorAt(0.0, QColor(250, 250, 200));
	gradient.setColorAt(0.25, QColor(249, 230, 150));
	gradient.setColorAt(0.5, QColor(247, 206, 91));
	gradient.setColorAt(0.75, QColor(240, 195, 80));
	gradient.setColorAt(1.0, QColor(232, 181, 66));

	p.setPen(QPen(QBrush(gradient), 0));
	p.drawText(_winnerLabelRect, Qt::AlignCenter, _winnerLabel);

	/** Create Rect to Draw Text Inside */
	qreal scale = 1.0 + 0.05 * std::sin(_animationPhase * 3.14);
	QRect winnerNameRect(0, 0, width() * 0.5, height() * 0.5);
	winnerNameRect.setSize(winnerNameRect.size() * scale);
	winnerNameRect.moveCenter(rect().center());

	/** Create Fitted Font */
	QFont fitted = util::FontUtil::fittedFont(_winnerNames, winnerNameRect, font());
	QFontMetrics fontMetrics(fitted);
	p.setRenderHint(QPainter::TextAntialiasing);
	p.setFont(fitted);

	/** Draw Text Shadow */
	shadowOffset = QPoint(10, 10);
	p.setPen(shadowColor);
	p.drawText(winnerNameRect.translated(shadowOffset), Qt::AlignCenter, _winnerNames);

	/** Draw Text */
	const QPointF startWinnerNames(winnerNameRect.center().x(), winnerNameRect.top());
	const QPointF endWinnerNames(winnerNameRect.center().x(), winnerNameRect.bottom());
	QLinearGradient textGradient(startWinnerNames, endWinnerNames);
	textGradient.setColorAt(0.0, QColor(255, 255, 255));
	textGradient.setColorAt(0.3, QColor(255, 255, 255));
	textGradient.setColorAt(1.0, QColor(210, 210, 210));

	p.setPen(QPen(QBrush(textGradient), 0));
	p.drawText(winnerNameRect, Qt::AlignCenter, _winnerNames);
}

void MusicQuiz::QuizWinningScreen::resizeEvent(QResizeEvent*)
{
	/** Update Particles */
	_particles.resize(rect());

	/** Update _winnerLabelFittedFont */
	_winnerLabelRect = QRect(0, height() * 0.18, width(), height() * 0.10);
	_winnerLabelFittedFont = util::FontUtil::fittedFont(_winnerLabel, _winnerLabelRect, font());

	/** Update Background Color */
	QRect r = rect();
	_backgroundColor = QRadialGradient(r.center(), r.width() * 0.8);
	int count = _winningTeams.size();
	for ( int i = 0; i < count; ++i ) {
		QColor color = _winningTeams[i]->getColor();
		qreal stop = count == 1 ? 0.0 : std::min(0.8, qreal(i) / ( count - 1 ));
		_backgroundColor.setColorAt(stop, color.lighter(115));
	}
	_backgroundColor.setColorAt(1.0, QColor("#0B1D3A")); // Creates a darker edge
}
