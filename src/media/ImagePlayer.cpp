#include "ImagePlayer.hpp"

#include <stdexcept>

#include <QVBoxLayout>

#include "common/Log.hpp"


media::ImagePlayer::ImagePlayer(QWidget* parent) :
	QWidget(parent)
{
	/** Create Image Widget */
	setObjectName("imagePlayerWidget");
	QVBoxLayout* layout = new QVBoxLayout;
	_imageLabel = new QLabel(this);
	_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_imageLabel->setAlignment(Qt::AlignCenter);
	layout->addWidget(_imageLabel);
	_imageLabel->show();
}

media::ImagePlayer::~ImagePlayer()
{
}

void media::ImagePlayer::showImage(const QString& imageFile)
{
	/** Sanity Check */
	if( imageFile.isEmpty() ) {
		throw std::runtime_error("Image File Name is empty.");
	}

	/** Set Image */
	QPixmap pixmap(imageFile);
	_imageLabel->setPixmap(pixmap.scaled(_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	show();
}

void media::ImagePlayer::resize(const QSize& size)
{
	/** Resize the video widget */
	if( _imageLabel != nullptr ) {
		_imageLabel->resize(size);
	}
}

void media::ImagePlayer::mousePressEvent(QMouseEvent* event)
{
	if ( _mouseEventCallback ) {
		_mouseEventCallback(event);
	}

	event->accept();
}

void media::ImagePlayer::keyPressEvent(QKeyEvent* event)
{
	/** Hide Video if esc is pressed */
	if ( event->key() == Qt::Key_Escape ) {
		hide();
	}

	event->accept();
}

void media::ImagePlayer::setMouseEventCallbackFunction(const std::function< void(QMouseEvent*) > mouseEventCallback)
{
	_mouseEventCallback = mouseEventCallback;
}