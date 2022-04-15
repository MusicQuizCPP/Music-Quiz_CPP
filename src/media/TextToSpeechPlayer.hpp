#pragma once

#include <memory>

#include <QString>
#include <QWidget>
#include <QObject>
#include <QTextToSpeech>


namespace media {
	class TextToSpeechPlayer : public QWidget {
		Q_OBJECT
	public:
		enum class TextToSpeechPlayState {
			IDLE = 1,		// Default
			PLAYING = 2,
			PAUSED = 3
		};

		/**
		 * @brief Constructor
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit TextToSpeechPlayer(QWidget* parent = nullptr);

		/**
		 * @brief destructor
		 */
		virtual ~TextToSpeechPlayer();

		/**
		 * @brief Shared Pointer
		 */
		typedef std::shared_ptr< TextToSpeechPlayer > Ptr;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		TextToSpeechPlayer(const TextToSpeechPlayer&) = delete;
		TextToSpeechPlayer& operator=(const TextToSpeechPlayer&) = delete;

		/**
		 * @brief Says a string.
		 *
		 * @param[in] string    The string to say.
		 * @param[in] startCharacter The character to start from.
		 */
		void play(const QString& string, size_t startCharacter = 0);

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
		void handleMediaStatus(QTextToSpeech::State status);

	protected:
		/** Variables */
		QTextToSpeech* _textToSpeech = nullptr;
		TextToSpeechPlayState _state = TextToSpeechPlayState::IDLE;
	};
}