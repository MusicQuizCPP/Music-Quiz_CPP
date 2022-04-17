#pragma once

#include <memory>
#include <string>

#include <QVoice>
#include <QString>
#include <QWidget>
#include <QObject>
#include <QVector>
#include <QTextToSpeech>


namespace media {
	class TextToSpeechPlayer : public QWidget {
		Q_OBJECT
	public:

		/** The text to speech settings */
		struct TextToSpeechSettings {
			/** Voice */
			QVoice _voice = QVoice();

			/** Voice pitch [-1.0, 1.0] */
			double _pitch = 0.0;

			/** Speec rate [-1.0, 1.0] */
			double _rate = 0.0;
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
		 * @param[in] string     The string to say.
		 * @param[in] settings   The text to speech settings.
		 * @param[in] playAnswer Set to true to play the answer part of the string.
		 */
		void play(const QString& string, const TextToSpeechSettings& settings, bool playAnswer = false);

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

		/**
		 * @brief Returns the list of available voices.
		 *
		 * @return The list of avaliable voices.
		 */
		QVector< QVoice > availableVoices() const;

	protected:
		/** Text to Speeh Widget */
		QTextToSpeech* _textToSpeech = nullptr;
	};
}