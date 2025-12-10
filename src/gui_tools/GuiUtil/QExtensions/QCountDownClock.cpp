#include "QCountDownClock.hpp"

#include <QPainter>
#include <QTimer>
#include <cmath>


MusicQuiz::QExtensions::QCountDownClock::QCountDownClock(const int countdown, const int widgetSize, QWidget* parent)
	: QWidget(parent), _countdown(countdown)
{	
	/** Sanity check */
	if ( _countdown <= 0 ) {
		throw std::runtime_error("The countdown cannot be 0 or less.");
	}

	/** Set size */
	setFixedSize(widgetSize, widgetSize);

	/** Setup windows */
	setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);

	/** Make background fully transparent (per-pixel) */
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_NoSystemBackground, true);

	/** Make widget/window transparent to mouse events (click-through) */
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setWindowFlag(Qt::WindowTransparentForInput, true);

	setAutoFillBackground(false);
	setStyleSheet("background: transparent;");

	/** Setup timer */
	_updateTimer = new QTimer(this);
	_updateTimer->setInterval(50);
	connect(_updateTimer, &QTimer::timeout, this, &QCountDownClock::updateClock);
}

void MusicQuiz::QExtensions::QCountDownClock::start()
{
	/** Start Timer */
	_elapsedTime = 0;
	show();
	_updateTimer->start();
	update();
}

void MusicQuiz::QExtensions::QCountDownClock::stop()
{
	/** Start Timer */
	_elapsedTime = 0;
	hide();
	_updateTimer->stop();
	update();
}

void MusicQuiz::QExtensions::QCountDownClock::updateClock()
{
	/** Update clock */
	_elapsedTime += _updateTimer->interval();
	update();

	/** Stop timer if countdown have been reached */
	if ( _elapsedTime >= _countdown ) {
		_updateTimer->stop();
	}
}

void MusicQuiz::QExtensions::QCountDownClock::paintEvent(QPaintEvent*) 
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	const QRectF rectangle = rect();
	const int startAngle = 90 * 16;
	const float value = 1.0f - static_cast<float>( _elapsedTime ) / static_cast<float>( _countdown );
	const int spanAngle = static_cast<int>(value * 360 * 16);

	/** Draw circle */
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(Qt::red));
	painter.drawEllipse(rectangle);

	/** Draw pie on top of circle */
	if ( _elapsedTime < _countdown ) {
		painter.setPen(Qt::NoPen);
		painter.setBrush(QBrush(Qt::lightGray));
		painter.drawPie(rectangle, startAngle, spanAngle);
	}

	/** Draw countdown text */
	painter.setBrush(Qt::NoBrush);
	QFont font = painter.font();
	font.setPixelSize(static_cast<int>(rectangle.height() / 2.5));
	font.setBold(true);
	painter.setFont(font);
	painter.setPen(Qt::yellow);
	const int timeLeft = std::ceil(( _countdown - _elapsedTime ) / 1000.0);
	painter.drawText(rectangle, Qt::AlignCenter, QString::number(timeLeft));
}