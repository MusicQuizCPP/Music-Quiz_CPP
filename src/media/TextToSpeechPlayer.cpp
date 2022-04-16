#include "TextToSpeechPlayer.hpp"

#include <stdexcept>

#include "common/Log.hpp"


media::TextToSpeechPlayer::TextToSpeechPlayer(QWidget* parent) :
	QWidget(parent)
{
	/** Create Media Player */
	_textToSpeech = new QTextToSpeech(this);
	_textToSpeech->setLocale(QLocale(QLocale::English, QLocale::LatinScript, QLocale::UnitedStates));
	_textToSpeech->setVolume(100);
}

media::TextToSpeechPlayer::~TextToSpeechPlayer()
{
	/** Stop Audio */
	_textToSpeech->stop();
	delete _textToSpeech;
}

void media::TextToSpeechPlayer::play(const QString& string, const TextToSpeechSettings& settings, bool playAnswer)
{
	/** Sanity Check */
	if ( string.isEmpty() ) {
		throw std::runtime_error("String is empty.");
	}

	/** Stop audio if any is playing and close file */
	stop();

	/** Set Voice */
	_textToSpeech->setVoice(settings._voice);

	/** Set Pitch */
	_textToSpeech->setPitch(settings._pitch);

	/** Set Rate */
	_textToSpeech->setRate(settings._rate);

	/** Get which part of the string should be played dependent on if it is an answer or not */
	QString speechString = string;
	if ( playAnswer ) {
		speechString = string.section('*', 1);
	}

	/** Remove "*" (if any remains or it is not the answer playing) */
	speechString.remove("*");

	/** Play Text */
	_textToSpeech->say(speechString);
}

void media::TextToSpeechPlayer::pause()
{
	_textToSpeech->pause();
}

void media::TextToSpeechPlayer::resume()
{
	_textToSpeech->resume();
}

void media::TextToSpeechPlayer::stop()
{
	_textToSpeech->stop();
}

QVector< QVoice > media::TextToSpeechPlayer::availableVoices() const
{
	return _textToSpeech->availableVoices();
}