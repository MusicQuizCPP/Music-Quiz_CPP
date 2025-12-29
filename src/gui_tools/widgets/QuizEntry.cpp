#include "QuizEntry.hpp"

#include <sstream>
#include <stdexcept>
#include <functional>

#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QHBoxLayout>
#include <QApplication>
#include <QFontDatabase>
#include <QStyleOptionButton>

#include "common/Log.hpp"
#include "util/FontUtil.hpp"


MusicQuiz::QuizEntry::QuizEntry(const std::filesystem::path& audioFile, const QString& answer, const size_t points, const size_t startTime, const size_t answerStartTime,
	const media::AudioPlayer::Ptr& audioPlayer, const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer,
	const media::ImagePlayer::Ptr& imagePlayer, const media::TextPlayer::Ptr& textPlayer, QWidget* parent) :
	QPushButton(parent), _points(points), _startTime(startTime), _answerStartTime(answerStartTime), _answer(answer), _audioFile(audioFile),
	_audioPlayer(audioPlayer), _videoPlayer(videoPlayer), _textToSpeechPlayer(textToSpeechPlayer), _imagePlayer(imagePlayer), _textPlayer(textPlayer)
{
	/** Sanity Check */
	if ( _audioPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid audio player.");
	}

	/** Initialize Widget */
	initializeWidget();

	/** Set Entry Type */
	_type = EntryType::Song;
}

MusicQuiz::QuizEntry::QuizEntry(const std::filesystem::path& audioFile, const std::filesystem::path& videoFile, const QString& answer, size_t points, size_t songStartTime,
	size_t videoStartTime, size_t answerStartTime, const media::AudioPlayer::Ptr& audioPlayer, const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer,
	const media::ImagePlayer::Ptr& imagePlayer, const media::TextPlayer::Ptr& textPlayer, QWidget* parent) :
	QPushButton(parent), _points(points), _startTime(songStartTime), _videoStartTime(videoStartTime),
	_answerStartTime(answerStartTime), _answer(answer), _audioFile(audioFile),
	_videoFile(videoFile), _audioPlayer(audioPlayer), _videoPlayer(videoPlayer), _textToSpeechPlayer(textToSpeechPlayer), _imagePlayer(imagePlayer), _textPlayer(textPlayer)
{
	/** Sanity Check */
	if ( _audioPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid audio player.");
	}

	if ( _videoPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid video player.");
	}

	/** Initialize Widget */
	initializeWidget();

	/** Set Entry Type */
	_type = EntryType::Video;

	/** Create Callback Function */
	_mouseEventCallback = std::bind(&MusicQuiz::QuizEntry::handleMouseEvent, this, std::placeholders::_1);
}

MusicQuiz::QuizEntry::QuizEntry(const QString& string, const std::filesystem::path& audioFile, const QString& answer, const size_t points, size_t answerStartTime,
	const media::AudioPlayer::Ptr& audioPlayer, const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer,
	const media::TextToSpeechPlayer::TextToSpeechSettings& textToSpeechSettings, const media::ImagePlayer::Ptr& imagePlayer, const media::TextPlayer::Ptr& textPlayer, QWidget* parent) :
	QPushButton(parent), _points(points), _speechString(string), _answerStartTime(answerStartTime), _answer(answer), _audioFile(audioFile),
	_audioPlayer(audioPlayer), _videoPlayer(videoPlayer), _textToSpeechPlayer(textToSpeechPlayer), _textToSpeechSettings(textToSpeechSettings), _imagePlayer(imagePlayer), _textPlayer(textPlayer)
{
	/** Sanity Check */	
	if ( _textToSpeechPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid text to speech player.");
	}

	/** Initialize Widget */
	initializeWidget();

	/** Set Entry Type */
	_type = EntryType::TextToSpeech;
}

MusicQuiz::QuizEntry::QuizEntry(const std::filesystem::path& imageFile, const std::filesystem::path& audioFile, const QString& answer, const size_t points, size_t answerStartTime,
	const int pixilationDuration, const media::AudioPlayer::Ptr& audioPlayer, const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer,
	const media::ImagePlayer::Ptr& imagePlayer, const media::TextPlayer::Ptr& textPlayer, QWidget* parent) :
	QPushButton(parent), _points(points), _imageFile(imageFile), _answerStartTime(answerStartTime), _answer(answer), _pixilationDuration(pixilationDuration), _audioFile(audioFile),
	_audioPlayer(audioPlayer), _videoPlayer(videoPlayer), _textToSpeechPlayer(textToSpeechPlayer), _imagePlayer(imagePlayer), _textPlayer(textPlayer)
{
	/** Sanity Check */
	if ( _audioPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid audio player.");
	}

	if ( _imagePlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid audio player.");
	}

	/** Initialize Widget */
	initializeWidget();

	/** Set Entry Type */
	_type = EntryType::Image;

	/** Create Callback Function */
	_mouseEventCallback = std::bind(&MusicQuiz::QuizEntry::handleMouseEvent, this, std::placeholders::_1);

	/** Connect signal */
	QObject::connect(_imagePlayer.get(), &media::ImagePlayer::shown, this, &MusicQuiz::QuizEntry::blurQuiz);
	QObject::connect(_imagePlayer.get(), &media::ImagePlayer::hidden, this, &MusicQuiz::QuizEntry::unBlurQuiz);
}

MusicQuiz::QuizEntry::QuizEntry(const QString& string, const std::filesystem::path& audioFile, const QString& answer, const size_t points, size_t answerStartTime,
	const media::AudioPlayer::Ptr& audioPlayer, const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer,
	const media::ImagePlayer::Ptr& imagePlayer, const media::TextPlayer::Ptr& textPlayer, QWidget* parent) :
	QPushButton(parent), _points(points), _textString(string), _answerStartTime(answerStartTime), _answer(answer), _audioFile(audioFile),
	_audioPlayer(audioPlayer), _videoPlayer(videoPlayer), _textToSpeechPlayer(textToSpeechPlayer), _imagePlayer(imagePlayer), _textPlayer(textPlayer)
{
	/** Sanity Check */
	if ( _audioPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid audio player.");
	}

	if ( _textPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid text player.");
	}

	/** Initialize Widget */
	initializeWidget();

	/** Set Entry Type */
	_type = EntryType::Text;

	/** Create Callback Function */
	_mouseEventCallback = std::bind(&MusicQuiz::QuizEntry::handleMouseEvent, this, std::placeholders::_1);

    /** Connect signal */
    QObject::connect(_textPlayer.get(), &media::TextPlayer::shown, this, &MusicQuiz::QuizEntry::blurQuiz);
    QObject::connect(_textPlayer.get(), &media::TextPlayer::hidden, this, &MusicQuiz::QuizEntry::unBlurQuiz);
}

void MusicQuiz::QuizEntry::initializeWidget()
{
	/** Set Button Text */
	setText("$" + QString::fromLocal8Bit(std::to_string(_points).c_str()));

	/** Set Start State */
	_state = EntryState::IDLE;

	/** Set Object Name */
	setObjectName("QuizEntry");

	/** Set Size Policy */
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	/** Set Cursor */
	setCursor(Qt::PointingHandCursor);

	/** Set font */
	const int fontIndex = QFontDatabase::addApplicationFont(":/fonts/BebasNeue-Regular.ttf");
	const QString fontFamily = QFontDatabase::applicationFontFamilies(fontIndex).at(0);

	QFont font(fontFamily);
	font.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
	setFont(font);
}

void MusicQuiz::QuizEntry::mouseReleaseEvent(QMouseEvent* event)
{
	/** Install Event Filter in Video Player Widget */
	if ( _videoPlayer != nullptr ) {
		_videoPlayer->setMouseEventCallbackFunction(_mouseEventCallback);
	}

	/** Install Event Filter in Image Player Widget */
	if ( _imagePlayer != nullptr ) {
		_imagePlayer->setMouseEventCallbackFunction(_mouseEventCallback);
	}

	/** Install Event Filter in Text Player Widget */
	if ( _textPlayer != nullptr ) {
		_textPlayer->setMouseEventCallbackFunction(_mouseEventCallback);
	}

	/** Handle Event */
	handleMouseEvent(event);
}

void MusicQuiz::QuizEntry::handleMouseEvent(QMouseEvent* event)
{
	if ( event->button() == Qt::LeftButton ) {
		leftClickEvent();
	} else if ( event->button() == Qt::RightButton ) {
		rightClickEvent();
	}

	/** Set Object Name (this changes the color) */
	switch ( _state ) {
	case MusicQuiz::QuizEntry::EntryState::IDLE:
		applyColor(_idleColor);
		break;
	case MusicQuiz::QuizEntry::EntryState::PLAYING:
		applyColor(_playingColor);
		break;
	case MusicQuiz::QuizEntry::EntryState::PAUSED:
		applyColor(_pausedColor);
		break;
	case MusicQuiz::QuizEntry::EntryState::PLAYING_ANSWER:
		applyColor(_playingAnswerColor);
		if ( !_entryAnswered ) {
			_entryAnswered = true;

			/** Points */
			size_t points = _points;
			if ( _doublePoints ) {
				points *= 2;
			} else if ( _triplePoints ) {
				points *= 3;
			}
			emit answered(points);
		}
		break;
	case MusicQuiz::QuizEntry::EntryState::PLAYED:
		emit played();
		applyColor(_answeredColor);
		break;
	default:
		break;
	}
}

void MusicQuiz::QuizEntry::leftClickEvent()
{
	switch ( _state ) {
	case EntryState::IDLE: // Start Media
		_state = EntryState::PLAYING;
		if ( _type == EntryType::Song ) {
			_videoPlayer->stop();
			_textToSpeechPlayer->stop();
			_audioPlayer->play(_audioFile, _startTime);
			_imagePlayer->hide();
			_textPlayer->hide();
		} else if ( _type == EntryType::Video ) {
			_audioPlayer->stop();
			_textToSpeechPlayer->stop();
			_imagePlayer->hide();
			_textPlayer->hide();
			_videoPlayer->play(_videoFile, _videoStartTime, true);
			_videoPlayer->show();
			_audioPlayer->play(_audioFile, _startTime);
		} else if ( _type == EntryType::TextToSpeech ) {
			_audioPlayer->stop();
			_videoPlayer->stop();
			_imagePlayer->hide();
			_textPlayer->hide();
			_textToSpeechPlayer->play(_speechString, _textToSpeechSettings);
		} else if ( _type == EntryType::Image ) {
			_audioPlayer->stop();
			_videoPlayer->stop();
			_textToSpeechPlayer->stop();
			_textPlayer->hide();
			_imagePlayer->showImage(_imageFile, _pixilationDuration);
		} else if ( _type == EntryType::Text ) {
			_audioPlayer->stop();
			_videoPlayer->stop();
			_textToSpeechPlayer->stop();
			_imagePlayer->hide();
			_textPlayer->showText(_textString);
		}

		break;
	case EntryState::PLAYING: // Pause Media
		_state = EntryState::PAUSED;

		_audioPlayer->pause();
		_videoPlayer->pause();
		_textToSpeechPlayer->pause();
		_imagePlayer->pause();
		emit startCountdown();

		break;
	case EntryState::PAUSED: // Play Answer
		emit stopCountdown();
		//_textSizeSet = false;
		_state = EntryState::PLAYING_ANSWER;
		if ( _type == EntryType::Song ) {
			_videoPlayer->stop();
			_textToSpeechPlayer->stop();
			_audioPlayer->play(_audioFile, _answerStartTime);
		} else if ( _type == EntryType::Video ) {
			_audioPlayer->stop();
			_textToSpeechPlayer->stop();
			_videoPlayer->play(_videoFile, _answerStartTime);
			_videoPlayer->show();
		} else if ( _type == EntryType::TextToSpeech ) {
			_videoPlayer->stop();
			_audioPlayer->play(_audioFile, _answerStartTime);
		} else if ( _type == EntryType::Image ) {
			_videoPlayer->stop();
			_textToSpeechPlayer->stop();
			_audioPlayer->play(_audioFile, _answerStartTime);
			_imagePlayer->showImage(_imageFile);
		} else if ( _type == EntryType::Text ) {
			_videoPlayer->stop();
			_textToSpeechPlayer->stop();
			_audioPlayer->play(_audioFile, _answerStartTime);
			_textPlayer->showText(_textString);
		}

		if ( !_hiddenAnswer ) {
			setText(QString::fromLocal8Bit(_answer.toStdString().c_str()));
		}
		break;
	case EntryState::PLAYING_ANSWER: // Entry Answered
		_state = EntryState::PLAYED;

		_audioPlayer->stop();
		_videoPlayer->stop();
		_videoPlayer->hide();
		_textToSpeechPlayer->stop();
		_imagePlayer->hide();
		_textPlayer->hide();

		break;
	case QuizEntry::EntryState::PLAYED: // Play Answer Again
		if ( _type == EntryType::Song ) {
			_videoPlayer->stop();
			_textToSpeechPlayer->stop();
			_audioPlayer->play(_audioFile, _answerStartTime);
		} else if ( _type == EntryType::Video ) {
			_audioPlayer->stop();
			_textToSpeechPlayer->stop();
			_videoPlayer->play(_videoFile, _answerStartTime);
			_videoPlayer->show();
		} else if ( _type == EntryType::TextToSpeech ) {
			_videoPlayer->stop();
			_audioPlayer->play(_audioFile, _answerStartTime);
		} else if ( _type == EntryType::Image ) {
			_audioPlayer->play(_audioFile, _answerStartTime);
			_imagePlayer->showImage(_imageFile);
		} else if ( _type == EntryType::Text ) {
			_audioPlayer->play(_audioFile, _answerStartTime);
			_textPlayer->showText(_textString);
		}

		_state = EntryState::PLAYING_ANSWER;
		break;
	default:
		throw std::runtime_error("Unknown Quiz Entry State Encountered.");
		break;
	}
}

void MusicQuiz::QuizEntry::rightClickEvent()
{
	switch ( _state ) {
	case EntryState::IDLE:
		break;
	case EntryState::PLAYING: // Back to initial state
		_state = EntryState::IDLE;

		_audioPlayer->pause();
		_videoPlayer->pause();
		_textToSpeechPlayer->pause();
		_imagePlayer->pause();

		if ( _type == EntryType::Video ) {
			_videoPlayer->show();
		}

		break;
	case EntryState::PAUSED: // Continue playing
		emit stopCountdown();
		_state = EntryState::PLAYING;
		
		if ( _type == EntryType::Song ) {
			_audioPlayer->resume();
		} else if ( _type == EntryType::Video ) {
			_videoPlayer->resume();
			_audioPlayer->resume();
			_videoPlayer->show();
		} else if ( _type == EntryType::TextToSpeech ) {
			_textToSpeechPlayer->resume();
		} else if ( _type == EntryType::Image ) {
			_imagePlayer->resume();
		}

		break;
	case EntryState::PLAYING_ANSWER: // Pause Media
		_state = EntryState::PAUSED;

		_audioPlayer->pause();
		_videoPlayer->pause();
		_textToSpeechPlayer->pause();

		if ( _type == EntryType::Video ) {
			_videoPlayer->show();
		}

		setText("$" + QString::fromLocal8Bit(std::to_string(_points).c_str()));
		break;
	case QuizEntry::EntryState::PLAYED: // Back to idle
		_entryAnswered = false;
		_state = EntryState::IDLE;
		setText("$" + QString::fromLocal8Bit(std::to_string(_points).c_str()));
		break;
	default:
		throw std::runtime_error("Unknown Quiz Entry State Encountered.");
		break;
	}
}

void MusicQuiz::QuizEntry::setColor(const QColor& color)
{
	/** Set Color */
	_answeredColor = color;
	_answeredColor.setAlpha(255);
}

void MusicQuiz::QuizEntry::applyColor(const QColor& color)
{
	/** Background Color */
	std::stringstream ss;
	ss << "background-color	: rgb(" << color.red() << ", " << color.green() << ", " << color.blue() << ", " << color.alpha() << ");";
	setStyleSheet(QString::fromStdString(ss.str()));

	/** Ensure icon border is not changed */
	QLabel* label = findChild<QLabel*>("QuizEntryCategoryIcon");
	if ( label != nullptr ) {
		label->setStyleSheet("border: none;");
	}
}

MusicQuiz::QuizEntry::EntryState MusicQuiz::QuizEntry::getEntryState()
{
	return _state;
}

void MusicQuiz::QuizEntry::setHiddenAnswer(bool hidden)
{
	_hiddenAnswer = hidden;
}

void MusicQuiz::QuizEntry::setDoublePointsEnabled(bool enabled, bool hidden)
{
	_doublePoints = enabled;
	_hiddenDoublePoints = hidden;
	if ( _doublePoints ) {
		_triplePoints = false;
	}
}

void MusicQuiz::QuizEntry::setTriplePointsEnabled(bool enabled, bool hidden)
{
	_triplePoints = enabled;
	_hiddenTriplePoints = hidden;
	if ( _triplePoints ) {
		_doublePoints = false;
	}
}

void MusicQuiz::QuizEntry::setShowEntryTypeIcon(bool showIcons)
{
	if ( showIcons ) {
		showEntryTypeIcon();
	}
}

void MusicQuiz::QuizEntry::showEntryTypeIcon()
{
	/** Create icon label */
	QLabel* iconLabel = new QLabel(this);
	const int iconSize = this->rect().width() * 0.05;
	iconLabel->setFixedSize(iconSize, iconSize);
	iconLabel->setObjectName("QuizEntryCategoryIcon");
	iconLabel->setStyleSheet("background: transparent;");
	iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

	/** Create icon layout */
	QHBoxLayout* iconLayout = new QHBoxLayout(this);
	iconLayout->addStretch();
	iconLayout->setContentsMargins(5, 5, 5, 5);
	iconLayout->addWidget(iconLabel, 0, Qt::AlignTop | Qt::AlignRight);

	/** Load icon based on entry type */
	QString iconPath = "";
	switch ( _type ) {
	case MusicQuiz::QuizEntry::EntryType::Song:
		iconPath = QStringLiteral(":/imgs/music_icon.png");
		break;
	case MusicQuiz::QuizEntry::EntryType::Video:
		iconPath = QStringLiteral(":/imgs/video_icon.png");
		break;
	case MusicQuiz::QuizEntry::EntryType::TextToSpeech:
		iconPath = QStringLiteral(":/imgs/text-to-speech_icon.png");
		break;
	case MusicQuiz::QuizEntry::EntryType::Image:
		iconPath = QStringLiteral(":/imgs/image_icon.png");
		break;
	case MusicQuiz::QuizEntry::EntryType::Text:
		iconPath = QStringLiteral(":/imgs/text_icon.png");
		break;
	default:
		return;
	}

	/** Load icon pixmap */
	QPixmap iconPixmap(iconPath);
	if ( iconPixmap.isNull() ) {
		return;
	}

	/** Scale icon to the correct size */
	const QPixmap scaledIconPixmap = iconPixmap.scaled(iconLabel->width(), iconLabel->width(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	/** Set icon pixmap */
	iconLabel->setPixmap(scaledIconPixmap);
}

void MusicQuiz::QuizEntry::paintEvent(QPaintEvent* event)
{
	/** Set up options and painter */
	QStyleOptionButton option;
	initStyleOption(&option);

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	/** Setup Colors */
	QColor topMiddleColor = QColor(13, 73, 171);
	QColor topCornersColor = QColor(14, 81, 189);
	QColor bottomCornersColor = QColor(111, 190, 225);
	QColor bottomMiddleColor = QColor(159, 210, 250);

	if ( _doublePoints ) { // Double Points
		if ( ( _hiddenDoublePoints && _state != QuizEntry::EntryState::IDLE ) || !_hiddenDoublePoints ) {
			topMiddleColor = QColor(220, 180, 0);
			topCornersColor = QColor(250, 200, 25);
			bottomCornersColor = QColor(250, 230, 25);
			bottomMiddleColor = QColor(250, 250, 25);
		}
	} else if ( _triplePoints ) { // Triple Points
		if ( ( _hiddenTriplePoints && _state != QuizEntry::EntryState::IDLE ) || !_hiddenTriplePoints ) {
			topMiddleColor = QColor(150, 0, 105);
			topCornersColor = QColor(180, 0, 135);
			bottomCornersColor = QColor(200, 0, 155);
			bottomMiddleColor = QColor(220, 0, 185);
		}
	}

	/** Draw base button */
	style()->drawControl(QStyle::CE_PushButtonBevel, &option, &p, this);
	style()->drawControl(QStyle::CE_PushButtonLabel, &option, &p, this);
	QRect r = rect();

	/** Top Line */
	QLinearGradient topGlow(r.topLeft(), r.topRight());
	topGlow.setColorAt(0.0, topCornersColor);
	topGlow.setColorAt(0.5, topMiddleColor);
	topGlow.setColorAt(1.0, topCornersColor);

	QPen topPen(topGlow, 5);
	topPen.setCapStyle(Qt::FlatCap);
	p.setPen(topPen);

	p.drawLine(QPoint(r.left(), r.top()), QPoint(r.right(), r.top()));

	/** Left Side */
	QLinearGradient leftGlow(r.topLeft(), r.bottomLeft());
	leftGlow.setColorAt(0.0, topCornersColor);
	leftGlow.setColorAt(1.0, bottomCornersColor);

	QPen leftPen(leftGlow, 5);
	leftPen.setCapStyle(Qt::FlatCap);
	p.setPen(leftPen);
	p.drawLine(QPoint(r.left(), r.top()), QPoint(r.left(), r.bottom()));

	/** Right Side */
	QLinearGradient rightGlow(r.topRight(), r.bottomRight());
	rightGlow.setColorAt(0.0, topCornersColor);
	rightGlow.setColorAt(1.0, bottomCornersColor);

	QPen rightPen(rightGlow, 5);
	rightPen.setCapStyle(Qt::FlatCap);
	p.setPen(rightPen);
	p.drawLine(QPoint(r.right(), r.top()), QPoint(r.right(), r.bottom()));

	/** Bottom */
	QLinearGradient bottomGlow(r.bottomLeft(), r.bottomRight());
	bottomGlow.setColorAt(0.0, bottomCornersColor);
	bottomGlow.setColorAt(0.5, bottomMiddleColor);
	bottomGlow.setColorAt(1.0, bottomCornersColor);

	QPen bottomPen(bottomGlow, 5);
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

	gradient.setColorAt(0.0, QColor(250, 250, 200));
	gradient.setColorAt(0.25, QColor(249, 230, 150));
	gradient.setColorAt(0.5, QColor(247, 206, 91));
	gradient.setColorAt(0.75, QColor(240, 195, 80));
	gradient.setColorAt(1.0, QColor(232, 181, 66));

	p.setPen(QPen(QBrush(gradient), 0));
	p.drawText(textRect, Qt::AlignCenter, text());
}