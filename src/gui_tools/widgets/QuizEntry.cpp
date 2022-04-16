#include "QuizEntry.hpp"

#include <sstream>
#include <stdexcept>
#include <functional>

#include <QPainter>
#include <QMouseEvent>
#include <QSizePolicy>

#include "common/Log.hpp"


MusicQuiz::QuizEntry::QuizEntry(const QString& audioFile, const QString& answer, const size_t points, const size_t startTime, const size_t answerStartTime,
	const media::AudioPlayer::Ptr& audioPlayer, const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer, QWidget* parent) :
	QPushButton(parent), _points(points), _startTime(startTime), _answerStartTime(answerStartTime), _answer(answer), _audioFile(audioFile),
	_audioPlayer(audioPlayer), _videoPlayer(videoPlayer), _textToSpeechPlayer(textToSpeechPlayer)
{
	/** Sanity Check */
	if ( _audioPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid audio player.");
	}

	/** Set Button Text */
	setText("$" + QString::fromLocal8Bit(std::to_string(_points).c_str()));

	/** Set Start State */
	_state = EntryState::IDLE;

	/** Set Object Name */
	setObjectName("QuizEntry");

	/** Set Size Policy */
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	/** Set Entry Type */
	_type = EntryType::Song;
}

MusicQuiz::QuizEntry::QuizEntry(const QString& audioFile, const QString& videoFile, const QString& answer, size_t points, size_t songStartTime, size_t videoStartTime, size_t answerStartTime,
	const media::AudioPlayer::Ptr& audioPlayer, const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer, QWidget* parent) :
	QPushButton(parent), _points(points), _startTime(songStartTime), _videoStartTime(videoStartTime),
	_answerStartTime(answerStartTime), _answer(answer), _audioFile(audioFile),
	_videoFile(videoFile), _audioPlayer(audioPlayer), _videoPlayer(videoPlayer), _textToSpeechPlayer(textToSpeechPlayer)
{
	/** Sanity Check */
	if ( _audioPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid audio player.");
	}

	if ( _videoPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid video player.");
	}

	/** Set Button Text */
	setText("$" + QString::fromLocal8Bit(std::to_string(_points).c_str()));

	/** Set Start State */
	_state = EntryState::IDLE;

	/** Set Object Name */
	setObjectName("QuizEntry");

	/** Set Size Policy */
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	/** Set Entry Type */
	_type = EntryType::Video;

	/** Create Callback Function */
	_mouseEventCallback = std::bind(&MusicQuiz::QuizEntry::handleMouseEvent, this, std::placeholders::_1);
}

MusicQuiz::QuizEntry::QuizEntry(const QString& string, const QString& answer, const size_t points, const size_t answerStartCharacter,
	const media::AudioPlayer::Ptr& audioPlayer, const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer,
	const media::TextToSpeechPlayer::TextToSpeechSettings& textToSpeechSettings, QWidget* parent) :
	QPushButton(parent), _points(points), _speechString(string), _answerStartCharacter(answerStartCharacter), _answer(answer),
	_audioPlayer(audioPlayer), _videoPlayer(videoPlayer), _textToSpeechPlayer(textToSpeechPlayer), _textToSpeechSettings(textToSpeechSettings)
{
	/** Sanity Check */
	if ( _textToSpeechPlayer == nullptr ) {
		throw std::runtime_error("Failed to create quiz entry. Invalid text to speech player.");
	}

	/** Set Button Text */
	setText("$" + QString::fromLocal8Bit(std::to_string(_points).c_str()));

	/** Set Start State */
	_state = EntryState::IDLE;

	/** Set Object Name */
	setObjectName("QuizEntry");

	/** Set Size Policy */
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	/** Set Entry Type */
	_type = EntryType::TextToSpeech;
}

void MusicQuiz::QuizEntry::mouseReleaseEvent(QMouseEvent* event)
{
	/** Install Event Filter in Video Player Widget */
	if ( _videoPlayer != nullptr ) {
		_videoPlayer->setMouseEventCallbackFunction(_mouseEventCallback);
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
		applyColor(QColor(0, 0, 255));
		break;
	case MusicQuiz::QuizEntry::EntryState::PLAYING:
		applyColor(QColor(0, 0, 139));
		break;
	case MusicQuiz::QuizEntry::EntryState::PAUSED:
		applyColor(QColor(255, 215, 0));
		break;
	case MusicQuiz::QuizEntry::EntryState::PLAYING_ANSWER:
		applyColor(QColor(0, 128, 0));
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
		} else if ( _type == EntryType::Video ) {
			_audioPlayer->stop();
			_textToSpeechPlayer->stop();
			_videoPlayer->play(_videoFile, _videoStartTime, true);
			_videoPlayer->show();
			_audioPlayer->play(_audioFile, _startTime);
		} else if ( _type == EntryType::TextToSpeech ) {
			_audioPlayer->stop();
			_videoPlayer->stop();
			_textToSpeechPlayer->play(_speechString, _textToSpeechSettings);
		}

		break;
	case EntryState::PLAYING: // Pause Media
		_state = EntryState::PAUSED;

		_audioPlayer->pause();
		_videoPlayer->pause();
		_textToSpeechPlayer->pause();

		break;
	case EntryState::PAUSED: // Play Answer
		_textSizeSet = false;
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
			_audioPlayer->stop();
			_videoPlayer->stop();
			_textToSpeechPlayer->play(_speechString, _textToSpeechSettings, true);
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
			_audioPlayer->stop();
			_videoPlayer->stop();
			_textToSpeechPlayer->play(_speechString, _textToSpeechSettings, true);
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

		if ( _type == EntryType::Video ) {
			_videoPlayer->show();
		}

		break;
	case EntryState::PAUSED: // Continue playing
		_state = EntryState::PLAYING;
		
		if ( _type == EntryType::Song ) {
			_audioPlayer->resume();
		} else if ( _type == EntryType::Video ) {
			_videoPlayer->resume();
			_audioPlayer->resume();
			_videoPlayer->show();
		} else if ( _type == EntryType::TextToSpeech ) {
			_textToSpeechPlayer->resume();
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

		_fontSize = 40;
		setText("$" + QString::fromLocal8Bit(std::to_string(_points).c_str()));
		break;
	case QuizEntry::EntryState::PLAYED: // Back to idle
		_fontSize = 40;
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
}

void MusicQuiz::QuizEntry::applyColor(const QColor& color)
{
	/** Background Color */
	std::stringstream ss;
	ss << "background-color	: rgb(" << color.red() << ", " << color.green() << ", " << color.blue() << ");";

	/** Border Color */
	QColor borderColor = color;
	if ( _doublePoints ) { // Double Points
		if ( (_hiddenDoublePoints && _state != QuizEntry::EntryState::IDLE) || !_hiddenDoublePoints ) {
			borderColor = QColor(255, 255, 0);
		}
	} else if ( _triplePoints ) { // Triple Points
		if ( (_hiddenTriplePoints && _state != QuizEntry::EntryState::IDLE) || !_hiddenTriplePoints ) {
			borderColor = QColor(220, 0, 185);
		}
	}
	ss << "border: 3px solid rgb(" << borderColor.red() << ", " << borderColor.green() << ", " << borderColor.blue() << ");";

	/** Text Color */
	if ( _state == QuizEntry::EntryState::PLAYED && color != QColor(128, 128, 128) ) {
		ss << "color : rgb(" << 255 - color.red() << ", " << 255 - color.green() << ", " << 255 - color.blue() << ");";
	} else {
		ss << "color : Yellow;";
	}

	/** Text Size */
	int textWidth = fontMetrics().boundingRect(text()).width();
	if ( _state != QuizEntry::EntryState::PLAYED && !_textSizeSet ) {
		size_t fontSize = 40;
		while ( textWidth > width() - 40 && fontSize > 10U ) {
			setStyleSheet("font-size: " + QString::number(fontSize) + "px;");
			textWidth = fontMetrics().boundingRect(text()).width();
			--fontSize;
		}

		_textSizeSet = true;
		_fontSize = fontSize;
	}
	ss << "font-size: " << _fontSize << "px;";

	setStyleSheet(QString::fromStdString(ss.str()));
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

	/** Apply Color */
	applyColor(QColor(0, 0, 255));
}

void MusicQuiz::QuizEntry::setTriplePointsEnabled(bool enabled, bool hidden)
{
	_triplePoints = enabled;
	_hiddenTriplePoints = hidden;
	if ( _triplePoints ) {
		_doublePoints = false;
	}

	/** Apply Color */
	applyColor(QColor(0, 0, 255));
}