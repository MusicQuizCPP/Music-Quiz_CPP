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

	/** Create timer */
	_updateTimer = new QTimer(this);
	_updateTimer->setInterval(50);
	connect(_updateTimer, &QTimer::timeout, this, &ImagePlayer::updateImagePixilation);
}

void media::ImagePlayer::showImage(const std::filesystem::path& imageFile, const int depixilationDuration)
{
	/** Sanity Check */
	if( imageFile.empty() ) {
		throw std::runtime_error("Image File Name is empty.");
	}

	/** Load Image */
	_originalPixmap = QPixmap(QString::fromStdString(imageFile.string()));
	if ( _originalPixmap.isNull() ) {
		throw std::runtime_error("Image File is invalid.");
	}

	/** Stop timer if it was running */
	_updateTimer->stop();

	/** If no depixilation, just show the image */
	if ( depixilationDuration == 0 ) {
		/** Set Image */
		_imageLabel->setPixmap(_originalPixmap.scaled(_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	} else {
		/** Set parameters */
		_elapsedTime = 0;
		_depixilationDuration = depixilationDuration;
		updateImagePixilation();

		/** Start update timer */
		_updateTimer->start();
	}

	/** Show image */
	show();
	emit shown();
}

void media::ImagePlayer::pause()
{
	if ( _depixilationDuration == 0 || !_updateTimer->isActive() ) {
		return;
	}

	_updateTimer->stop();
}

void media::ImagePlayer::resume()
{
	if ( _depixilationDuration == 0 ) {
		return;
	}

	_updateTimer->start();
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
	_updateTimer->stop();
	emit hidden();
	event->accept();
}

void media::ImagePlayer::hideEvent(QHideEvent* event)
{
	_updateTimer->stop();
	emit hidden();
}

void media::ImagePlayer::updateImagePixilation()
{
    /** Sanity Checks */
    if ( _depixilationDuration <= 0 ) {
        _updateTimer->stop();
        return;
    }

    if ( _imageLabel == nullptr ) {
        _updateTimer->stop();
        return;
    }

    const QSize labelSize = _imageLabel->size();
    if ( labelSize.width() <= 0 || labelSize.height() <= 0 ) {
        _updateTimer->stop();
        return;
    }

    /** Update elapsed time */
    _elapsedTime += _updateTimer->interval();

    /** Check if depixilation is complete */
    if ( _elapsedTime >= _depixilationDuration ) {
        _imageLabel->setPixmap(_originalPixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        _updateTimer->stop();
        return;
    }

	/** Calculate progress */
	const double progress = static_cast<double>(_elapsedTime) / static_cast<double>(_depixilationDuration);

	/** Use an ease - in curve to slow the early normalization(progress ^ 2). */
	const double easedProgress = std::pow(progress, 2.0);

	/** Interpolate factor from minFactor -> 1.0 using eased progress */
	const double minimumFactor = 0.005;
	const double factor = minimumFactor + easedProgress * ( 1.0 - minimumFactor );

	/** Calculate new size */
    int newWidth = static_cast<int>( labelSize.width() * factor );
    int newHeight = static_cast<int>( labelSize.height() * factor );
    newWidth = std::max(1, newWidth);
    newHeight = std::max(1, newHeight);
    const QSize newSize(newWidth, newHeight);

	/** Downscale original to small size(keep aspect ratio), then scale back up. */
    const QPixmap downScaledImage = _originalPixmap.scaled(newSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    const QPixmap upScaledImage = downScaledImage.scaled(labelSize, Qt::KeepAspectRatio, Qt::FastTransformation);

	/** Update image */
	_imageLabel->setPixmap(upScaledImage);
}