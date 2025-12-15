#pragma once

#include <memory>

#include <QString>
#include <QWidget>
#include <QObject>
#include <QMediaPlayer>


namespace media {
	class AudioPlayer : public QWidget {
		Q_OBJECT

	public:
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
		 * @brief Plays a song.
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
		 * @brief Handles media status changes
		 */
		void handleMediaStatus(QMediaPlayer::MediaStatus status);

	protected:
		/** Audio Play State */
		enum class AudioPlayState {
			IDLE = 1,		// Default
			PLAYING = 2,
			PAUSED = 3
		};

		/** Variables */
		QMediaPlayer* _player = nullptr;
		AudioPlayState _state = AudioPlayState::IDLE;
	};
}