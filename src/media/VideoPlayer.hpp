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
	class VideoPlayer : public QWidget {
		Q_OBJECT
	public:
		enum class VideoPlayState {
			IDLE = 1,		// Default
			PLAYING = 2,
			PAUSED = 3
		};

		/**
		 * @brief Constructor
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit VideoPlayer(QWidget* parent = nullptr);

		/**
		 * @brief destructor
		 */
		virtual ~VideoPlayer();

		/**
		 * @brief Shared Pointer
		 */
		typedef std::shared_ptr< VideoPlayer > Ptr;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		VideoPlayer(const VideoPlayer&) = delete;
		VideoPlayer& operator=(const VideoPlayer&) = delete;

		/**
		 * @brief Plays a video.
		 *
		 * @param[in] videoFile The name of the video file to play.
		 * @param[in] startTime The time at which to start playing the video file from.
		 * @param[in] muted True if the audio should be muted.
		 */
		void play(const QString& videoFile, size_t startTime = 0, bool muted = false);

		/**
		 * @brief Pauses the video that is currently playing.
		 */
		void pause();

		/**
		 * @brief Resumes the video if it was paused.
		 */
		void resume();

		/**
		 * @brief Stops the video.
		 */
		void stop();

		/**
		 * @brief Resize the widget.
		 *
		 * @param[in] size The new size.
		 */
		void resize(const QSize& size);

		/**
		 * @brief Set the mouse event call back function.
		 *
		 * @param[in] mouseEventCallback The callback function.
		 */
		void setMouseEventCallbackFunction(const std::function< void(QMouseEvent*) > mouseEventCallback);

	private slots:
		/**
		 * @brief Handles mediastatus changes
		 */
		void handleMediaStatus(QMediaPlayer::MediaStatus status);

	protected:
		/**
		 * @brief Override the mouse release event.
		 *
		 * @param[in] event The event.
		 */
		void mousePressEvent(QMouseEvent* event) override;

		/**
		 * @brief Override the key press event.
		 *
		 * @param[in] event The event.
		 */
		void keyPressEvent(QKeyEvent* event) override;

		/** Variables */
		QMediaPlayer* _player = nullptr;
		QVideoWidget* _videoWidget = nullptr;
		VideoPlayState _state = VideoPlayState::IDLE;

		std::function< void(QMouseEvent*) > _mouseEventCallback;
	};
}