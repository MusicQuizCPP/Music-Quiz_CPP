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
	_imageLabel->setObjectName("ImagePlayerLabel");
	_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_imageLabel->setAlignment(Qt::AlignCenter);
	layout->addWidget(_imageLabel);
	_imageLabel->show();
}

void media::ImagePlayer::showImage(const std::filesystem::path& imageFile)
{
	/** Sanity Check */
	if( imageFile.empty() ) {
		throw std::runtime_error("Image File Name is empty.");
	}

	/** Set Image */
	QPixmap pixmap(QString::fromStdString(imageFile.string()));
	_imageLabel->setPixmap(pixmap.scaled(_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	show();
	emit shown();
}

void media::ImagePlayer::resize(const QSize& size)
{
	/** Resize the text widget */
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
	/** Hide image if esc is pressed */
	if ( event->key() == Qt::Key_Escape ) {
		hide();
	}

	event->accept();
}

void media::ImagePlayer::setMouseEventCallbackFunction(const std::function< void(QMouseEvent*) > mouseEventCallback)
{
	_mouseEventCallback = mouseEventCallback;
}

void media::ImagePlayer::closeEvent(QCloseEvent* event)
{
	emit hidden();
	event->accept();
}

void media::ImagePlayer::hideEvent(QHideEvent* event)
{
	emit hidden();
}