#include "TextPlayer.hpp"

#include <utility>
#include <stdexcept>

#include <QScreen>
#include <QVBoxLayout>
#include <QGuiApplication>

#include "common/Log.hpp"


media::TextPlayer::TextPlayer(QWidget* parent) :
	QWidget(parent)
{
	/** Create Text Widget */
	setObjectName("TextPlayerWidget");
	QVBoxLayout* layout = new QVBoxLayout;
	_textLabel = new MusicQuiz::QExtensions::QLabelExtender(this);
	_textLabel->setObjectName("TextPlayerLabel");
	_textLabel->setAlignment(Qt::AlignCenter);
	_textLabel->setWordWrap(true); 
	layout->addWidget(_textLabel);
	_textLabel->show();
}

void media::TextPlayer::showText(const QString& textString)
{
	/** Sanity Check */
	if( textString.isEmpty() ) {
		throw std::runtime_error("Text string is empty.");
	}

	/** Update position */
	move(QGuiApplication::primaryScreen()->geometry().center() - rect().center());

	/** Set Text */
	_textLabel->setText(textString);
	_textLabel->updateLayout();

	/** Update position */
	move(QGuiApplication::primaryScreen()->geometry().center() - rect().center());

	/** Show widget */
	show();
	emit shown();
}

void media::TextPlayer::resize(const QSize& size)
{
	/** Resize the text widget */
	if( _textLabel != nullptr ) {
		_textLabel->resize(size);
	}
}

void media::TextPlayer::mousePressEvent(QMouseEvent* event)
{
	if ( _mouseEventCallback ) {
		_mouseEventCallback(event);
	}

	event->accept();
}

void media::TextPlayer::keyPressEvent(QKeyEvent* event)
{
	/** Hide text if esc is pressed */
	if ( event->key() == Qt::Key_Escape ) {
		hide();
	}

	event->accept();
}

void media::TextPlayer::setMouseEventCallbackFunction(const std::function< void(QMouseEvent*) > mouseEventCallback)
{
	_mouseEventCallback = mouseEventCallback;
}

void media::TextPlayer::closeEvent(QCloseEvent* event)
{
	emit hidden();
	event->accept();
}

void media::TextPlayer::hideEvent(QHideEvent* event)
{
	emit hidden();
}