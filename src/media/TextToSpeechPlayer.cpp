#include "TextToSpeechPlayer.hpp"

#include <stdexcept>

#include <QVoice>

#include "common/Log.hpp"


media::TextToSpeechPlayer::TextToSpeechPlayer(QWidget* parent) :
	QWidget(parent)
{
	/** Create Media Player */
	_textToSpeech = new QTextToSpeech(this);
	_textToSpeech->setLocale(QLocale(QLocale::English, QLocale::LatinScript, QLocale::UnitedStates));
	_textToSpeech->setVolume(100);
	connect(_textToSpeech, &QTextToSpeech::stateChanged, this, &media::TextToSpeechPlayer::handleMediaStatus);
}

media::TextToSpeechPlayer::~TextToSpeechPlayer()
{
	/** Stop Audio */
	_textToSpeech->stop();
	delete _textToSpeech;
}

void media::TextToSpeechPlayer::play(const QString& string, const size_t startCharacter)
{
	/** Sanity Check */
	if ( string.isEmpty() ) {
		throw std::runtime_error("String is empty.");
	}

	/** Stop audio if any is playing and close file */
	stop();

	/** Set Audio File */
	_textToSpeech->say(string);
	//_textToSpeech->setMedia(QUrl::fromLocalFile(audioFile));

	/** Set Start Time */
	//_textToSpeech->setPosition(startTime);

	/** Pause Audio */
	_textToSpeech->pause();

	/** Set State */
	_state = TextToSpeechPlayState::PAUSED;

	//The playback will be started in handleMediaStatus when file has been loaded.
}

void media::TextToSpeechPlayer::pause()
{
	/** Check State */
	if ( _state != TextToSpeechPlayState::PLAYING ) {
		return;
	}

	/** Pause */
	_textToSpeech->pause();

	/** Set State */
	_state = TextToSpeechPlayState::PAUSED;
}

void media::TextToSpeechPlayer::resume()
{
	/** Check State */
	if ( _state != TextToSpeechPlayState::PAUSED ) {
		return;
	}

	/** Resume Audio */
	_textToSpeech->resume();

	/** Set State */
	_state = TextToSpeechPlayState::PLAYING;
}

void media::TextToSpeechPlayer::stop()
{
	/** Stop */
	_textToSpeech->stop();

	/** Set State */
	_state = TextToSpeechPlayState::IDLE;
}

void media::TextToSpeechPlayer::handleMediaStatus(QTextToSpeech::State status)
{
	/** Check State */
	if ( _state != TextToSpeechPlayState::PAUSED ) {
		return;
	}

	if ( status == QTextToSpeech::State::Ready ) {
		this->resume();
	}
}