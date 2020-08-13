#pragma once

#include <memory>

#include <QString>
#include <QWidget>
#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QVideoWidget>


namespace media {
	class AudioPlayer : public QWidget {
		Q_OBJECT
	public:
		enum class AudioPlayState {
			IDLE = 1,		// Default
			PLAYING = 2,
			PAUSED = 3
		};

		/**
		 * @brief Constructor
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit AudioPlayer(QWidget* parent = nullptr);

		/**
		 * @brief destructor
		 */
		virtual ~AudioPlayer();

		/**
		 * @brief Shared Pointer
		 */
		typedef std::shared_ptr< AudioPlayer > Ptr;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		AudioPlayer(const AudioPlayer&) = delete;
		AudioPlayer& operator=(const AudioPlayer&) = delete;

		/**
		 * @brief Plays a video.
		 *
		 * @param[in] audioFile The name of the audio file to play.
		 * @param[in] startTime The time at which to start playing the audio file from.
		 */
		void play(const QString& audioFile, size_t startTime = 0);

		/**
		 * @brief Pauses the audio that is currently playing.
		 */
		void pause();

		/**
		 * @brief Resumes the audio if it was paused.
		 */
		void resume();

		/**
		 * @brief Stops the audio.
		 */
		void stop();

	private slots:
		/**
		 * @brief Handles mediastatus changes
		 */
		void handleMediaStatus(QMediaPlayer::MediaStatus status);

	protected:

		/** Variables */
		QMediaPlayer* _player = nullptr;
		AudioPlayState _state = AudioPlayState::IDLE;
	};
}