#include "AudioPlayer.hpp"

#include <stdexcept>

#include <QVBoxLayout>
#include <QMediaContent>

#include "common/Log.hpp"


media::AudioPlayer::AudioPlayer(QWidget* parent) :
	QWidget(parent)
{
	/** Create Media Player */
	_player = new QMediaPlayer(this);
	_player->setVolume(100);
	connect(_player, &QMediaPlayer::mediaStatusChanged, this, &media::AudioPlayer::handleMediaStatus);
}

media::AudioPlayer::~AudioPlayer()
{
	/** Stop Audio */
	_player->stop();
	delete _player;
}

void media::AudioPlayer::play(const QString& audioFile, const size_t startTime)
{
	/** Sanity Check */
	if ( audioFile.isEmpty() ) {
		throw std::runtime_error("Video File Name is empty.");
	}

	/** Stop audio if any is playing and close file */
	stop();

	/** Set Audio File */
	_player->setMedia(QUrl::fromLocalFile(audioFile));

	/** Set Start Time */
	_player->setPosition(startTime);

	/** Pause Audio */
	_player->pause();

	/** Set State */
	_state = AudioPlayState::PAUSED;

	//The playback will be started in handleMediaStatus when file has been loaded.
}

void media::AudioPlayer::pause()
{
	/** Check State */
	if ( _state != AudioPlayState::PLAYING ) {
		return;
	}

	/** Pause */
	_player->pause();

	/** Set State */
	_state = AudioPlayState::PAUSED;
}

void media::AudioPlayer::resume()
{
	/** Check State */
	if ( _state != AudioPlayState::PAUSED ) {
		return;
	}

	/** Resume Audio */
	_player->play();

	/** Set State */
	_state = AudioPlayState::PLAYING;
}

void media::AudioPlayer::stop()
{
	/** Stop Video */
	_player->stop();
	_player->setMedia(QMediaContent());

	/** Set State */
	_state = AudioPlayState::IDLE;
}

void media::AudioPlayer::handleMediaStatus(QMediaPlayer::MediaStatus status)
{
	/** Check State */
	if ( _state != AudioPlayState::PAUSED ) {
		return;
	}

	if ( status == QMediaPlayer::MediaStatus::BufferedMedia || status == QMediaPlayer::MediaStatus::LoadedMedia ) {
		this->resume();
	}
}