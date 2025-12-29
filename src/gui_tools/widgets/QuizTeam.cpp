#include "QuizTeam.hpp"

#include <sstream>
#include <stdlib.h>
#include <stdexcept>

#include <QPainter>
#include <QMouseEvent>
#include <QFontDatabase>
#include <QStyleOptionButton>

#include "common/Log.hpp"
#include "util/FontUtil.hpp"


MusicQuiz::QuizTeam::QuizTeam(const QString& name, const QColor& color, QWidget* parent) :
	QPushButton(parent), _name(name), _score(0), _color(color), _newPoints(0),
	_scoreCntRate(1), _scoreTimerDelayMs(25), _hideScore(false)
{
	/** Sanity Check */
	if ( _name.isEmpty() ) {
		throw std::runtime_error("Cannot create team without a name.");
	}

    /** Set font */
    const int fontIndex = QFontDatabase::addApplicationFont(":/fonts/BebasNeue-Regular.ttf");
    const QString fontFamily = QFontDatabase::applicationFontFamilies(fontIndex).at(0);

    QFont font(fontFamily);
    font.setCapitalization(QFont::AllUppercase);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
    setFont(font);

	/** Set Team Text */
	QString str = _name + ": " + QString::fromLocal8Bit(std::to_string(_score).c_str()) + "$";
	setText(str);

	/** Set Background Color */
	std::stringstream ss;
	ss << "background-color	: rgb(" << _color.red() << ", " << _color.green() << ", " << _color.blue() << ");";
	setStyleSheet(QString::fromStdString(ss.str()));

	/** Set Object Name */
	setObjectName("TeamEntry");

	/** Connect timer */
	connect(&_scoreCntTimer, SIGNAL(timeout()), this, SLOT(accumulateScore()));
}

MusicQuiz::QuizTeam::~QuizTeam()
{
	/** Stop Timer */
	if ( !_scoreCntTimer.isActive() ) {
		_scoreCntTimer.stop();
	}
}

void MusicQuiz::QuizTeam::setHideScore(bool hide)
{
	_hideScore = hide;

	/** Update Name */
	QString str = _name + (_hideScore ? "" : ": " + QString::fromLocal8Bit(std::to_string(_score).c_str()) + "$");
	setText(str);
}

void MusicQuiz::QuizTeam::addPoints(size_t points)
{
	/** Update Score */
	if ( !_hideScore ) {
		_newPoints += points;
		LOG_INFO(_scoreTimerDelayMs.count());
		_scoreCntRate = _newPoints / _scoreTimerDelayMs.count();
	} else {
		_score += points;
	}

	if ( !_scoreCntTimer.isActive() ) {
		_scoreCntTimer.start(_scoreTimerDelayMs);
	}
}

void MusicQuiz::QuizTeam::accumulateScore()
{
	/** Get random number to add to score */
	size_t val = rand() % _scoreCntRate + 1;
	if ( val > _newPoints ) {
		val = _newPoints;
	}

	/** Add Score */
	_score += val;
	_newPoints -= val;

	/** Update Text */
	QString str = _name + (_hideScore ? "" : ": " + QString::fromLocal8Bit(std::to_string(_score).c_str()) + "$");
	setText(str);

	/** Check if points have been added */
	if ( _newPoints <= 0 ) {
		_scoreCntTimer.stop();
	}
}

QString MusicQuiz::QuizTeam::getName() const
{
	return _name;
}

size_t MusicQuiz::QuizTeam::getScore() const
{
	return _score + _newPoints;
}

QColor MusicQuiz::QuizTeam::getColor() const
{
	return _color;
}

void MusicQuiz::QuizTeam::paintEvent(QPaintEvent* event)
{
    /** Set up options and painter */
    QStyleOptionButton option;
    initStyleOption(&option);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    /** Draw base button */
    style()->drawControl(QStyle::CE_PushButtonBevel, &option, &p, this);
    style()->drawControl(QStyle::CE_PushButtonLabel, &option, &p, this);

    QRect r = rect();

    /** Top Line */
    QLinearGradient topGlow(r.topLeft(), r.topRight());
    topGlow.setColorAt(0.0, QColor(14, 81, 189));
    topGlow.setColorAt(0.5, QColor(13, 73, 171));
    topGlow.setColorAt(1.0, QColor(14, 81, 189));

    QPen topPen(topGlow, 5);
    topPen.setCapStyle(Qt::FlatCap);
    p.setPen(topPen);
    p.drawLine(QPoint(r.left(), r.top()), QPoint(r.right(), r.top()));

    /** Left Side */
    QLinearGradient leftGlow(r.topLeft(), r.bottomLeft());
    leftGlow.setColorAt(0.0, QColor(14, 81, 189));
    leftGlow.setColorAt(1.0, QColor(141, 223, 245));

    QPen leftPen(leftGlow, 7);
    leftPen.setCapStyle(Qt::FlatCap);
    p.setPen(leftPen);
    p.drawLine(QPoint(r.left(), r.top()), QPoint(r.left(), r.bottom()));

    /** Right Side */
    QLinearGradient rightGlow(r.topRight(), r.bottomRight());
    rightGlow.setColorAt(0.0, QColor(14, 81, 189));
    rightGlow.setColorAt(1.0, QColor(141, 223, 245));

    QPen rightPen(rightGlow, 7);
    rightPen.setCapStyle(Qt::FlatCap);
    p.setPen(rightPen);
    p.drawLine(QPoint(r.right(), r.top()), QPoint(r.right(), r.bottom()));

    /** Bottom */
    QLinearGradient bottomGlow(r.bottomLeft(), r.bottomRight());
    bottomGlow.setColorAt(0.0, QColor(141, 223, 245));
    bottomGlow.setColorAt(0.45, QColor(231, 252, 249));
    bottomGlow.setColorAt(0.5, QColor(255, 255, 255));
    bottomGlow.setColorAt(0.55, QColor(231, 252, 249));
    bottomGlow.setColorAt(1.0, QColor(141, 223, 245));

    QPen bottomPen(bottomGlow, 10);
    bottomPen.setCapStyle(Qt::FlatCap);
    p.setPen(bottomPen);
    p.drawLine(QPoint(r.left(), r.bottom()), QPoint(r.right(), r.bottom()));

    /** Get Text Rect */
    const QRect textRect = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);

    /** Create Fitted Font */
    QFont baseFont = font();
    QFont fitted = util::FontUtil::fittedFont(text(), textRect, baseFont);
    QFontMetrics fontMetrics(fitted);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setFont(fitted);

    /** Draw Text Shadow */
    QPoint shadowOffset(4, 4);
    QColor shadowColor(0, 0, 0, 150);

    p.setPen(shadowColor);
    p.drawText(textRect.translated(shadowOffset), Qt::AlignCenter, text());

    /** Draw Text */
    const QPointF start(textRect.center().x(), textRect.top());
    const QPointF end(textRect.center().x(), textRect.bottom());
    QLinearGradient gradient(start, end);
    gradient.setColorAt(0.0, QColor(230, 230, 230));
    gradient.setColorAt(1.0, QColor(255, 255, 255));

    p.setPen(QPen(QBrush(gradient), 0));
    p.drawText(textRect, Qt::AlignCenter, text());
}