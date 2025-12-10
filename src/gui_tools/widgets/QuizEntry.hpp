#pragma once

#include <string>
#include <memory>
#include <filesystem>

#include <QString>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPushButton>

#include "media/AudioPlayer.hpp"
#include "media/VideoPlayer.hpp"
#include "media/TextToSpeechPlayer.hpp"
#include "media/ImagePlayer.hpp"
#include "media/TextPlayer.hpp"

#include "common/Log.hpp"


class QMouseEvent;

namespace MusicQuiz {
	class QuizEntry : public QPushButton {
		Q_OBJECT

	public:
		/**
		 * @brief Entry Type Song Constructor
		 *
		 * @param[in] audioFile          The audio file to play.
		 * @param[in] answer             The entry anwser.
		 * @param[in] points             The number of points obtained by guessing the entry.
		 * @param[in] startTime          The start time of the media in [ms].
		 * @param[in] answerStartTime    The answer media start time in [ms].
		 * @param[in] audioPlayer        The audio player.
		 * @param[in] videoPlayer        The video player.
		 * @param[in] textToSpeechPlayer The text to speech player.
		 * @param[in] imagePlayer        The image player.
		 * @param[in] textPlayer         The text player.
		 * @param[in] parent             The parent widget.
		 */
		explicit QuizEntry(const std::filesystem::path& audioFile, const QString& answer, size_t points, size_t startTime, size_t answerStartTime,
			const std::shared_ptr< media::AudioPlayer >& audioPlayer, const std::shared_ptr< media::VideoPlayer >& videoPlayer,
			const std::shared_ptr< media::TextToSpeechPlayer >& textToSpeechPlayer, const std::shared_ptr< media::ImagePlayer >& imagePlayer,
			const std::shared_ptr< media::TextPlayer >& textPlayer, QWidget* parent = nullptr);

		/**
		 * @brief Entry Type Video Constructor
		 *
		 * @param[in] audioFile          The audio file to play.
		 * @param[in] videoFile          The audio file to play.
		 * @param[in] answer             The entry anwser.
		 * @param[in] points             The number of points obtained by guessing the entry.
		 * @param[in] songStartTime      The start time of the media in [ms].
		 * @param[in] videoStartTime     The start time of the media in [ms].
		 * @param[in] answerStartTime    The answer media start time in [ms].
		 * @param[in] audioPlayer        The audio player.
		 * @param[in] videoPlayer        The video player.
		 * @param[in] textToSpeechPlayer The text to speech player.
		 * @param[in] imagePlayer        The image player.
		 * @param[in] textPlayer         The text player.
		 * @param[in] parent             The parent widget.
		 */
		explicit QuizEntry(const std::filesystem::path& audioFile, const std::filesystem::path& videoFile, const QString& answer, size_t points, size_t songStartTime,
			size_t videoStartTime, size_t answerStartTime, const std::shared_ptr< media::AudioPlayer >& audioPlayer, const std::shared_ptr< media::VideoPlayer >& videoPlayer,
			const std::shared_ptr< media::TextToSpeechPlayer >& textToSpeechPlayer, const std::shared_ptr< media::ImagePlayer >& imagePlayer,
			const std::shared_ptr< media::TextPlayer >& textPlayer, QWidget* parent = nullptr);

		/**
		 * @brief Entry Type Text to Speech Constructor
		 *
		 * @param[in] string               The string to say.
		 * @param[in] audioFile            The answer audio file to play.
		 * @param[in] answer               The entry anwser.
		 * @param[in] points               The number of points obtained by guessing the entry.
		 * @param[in] answerStartTime      The answer media start time in [ms].
		 * @param[in] audioPlayer          The audio player.
		 * @param[in] videoPlayer          The video player.
		 * @param[in] textToSpeechPlayer   The text to speech player.
		 * @param[in] textToSpeechSettings The text to speech settings.
		 * @param[in] imagePlayer          The image player.
		 * @param[in] textPlayer           The text player.
		 * @param[in] parent               The parent widget.
		 */
		explicit QuizEntry(const QString& string, const std::filesystem::path& audioFile, const QString& answer, size_t points, size_t answerStartTime,
			const std::shared_ptr< media::AudioPlayer >& audioPlayer, const std::shared_ptr< media::VideoPlayer >& videoPlayer,
			const std::shared_ptr< media::TextToSpeechPlayer >& textToSpeechPlayer, const media::TextToSpeechPlayer::TextToSpeechSettings& textToSpeechSettings, 
			const std::shared_ptr< media::ImagePlayer >& imagePlayer, const std::shared_ptr< media::TextPlayer >& textPlayer, QWidget* parent = nullptr);

		/**
		 * @brief Entry Type Image Constructor
		 *
		 * @param[in] imageFile            The image file to show.
		 * @param[in] audioFile            The answer audio file to play.
		 * @param[in] answer               The entry anwser.
		 * @param[in] points               The number of points obtained by guessing the entry.
		 * @param[in] answerStartTime      The answer media start time in [ms].
		 * @param[in] audioPlayer          The audio player.
		 * @param[in] videoPlayer          The video player.
		 * @param[in] textToSpeechPlayer   The text to speech player.
		 * @param[in] imagePlayer          The image player.
		 * @param[in] textPlayer           The text player.
		 * @param[in] parent               The parent widget.
		 */
		explicit QuizEntry(const std::filesystem::path& imageFile, const std::filesystem::path& audioFile, const QString& answer, size_t points, size_t answerStartTime,
			const std::shared_ptr< media::AudioPlayer >& audioPlayer, const std::shared_ptr< media::VideoPlayer >& videoPlayer,
			const std::shared_ptr< media::TextToSpeechPlayer >& textToSpeechPlayer, const std::shared_ptr< media::ImagePlayer >& imagePlayer,
			const std::shared_ptr< media::TextPlayer >& textPlayer, QWidget* parent = nullptr);

		/**
		 * @brief Entry Type Text Constructor
		 *
		 * @param[in] string               The string to say.
		 * @param[in] audioFile            The answer audio file to play.
		 * @param[in] answer               The entry anwser.
		 * @param[in] points               The number of points obtained by guessing the entry.
		 * @param[in] answerStartTime      The answer media start time in [ms].
		 * @param[in] audioPlayer          The audio player.
		 * @param[in] videoPlayer          The video player.
		 * @param[in] textToSpeechPlayer   The text player.
		 * @param[in] imagePlayer          The image player.
		 * @param[in] textPlayer           The text player.
		 * @param[in] parent               The parent widget.
		 */
		explicit QuizEntry(const QString& string, const std::filesystem::path& audioFile, const QString& answer, size_t points, size_t answerStartTime,
			const std::shared_ptr< media::AudioPlayer >& audioPlayer, const std::shared_ptr< media::VideoPlayer >& videoPlayer,
			const std::shared_ptr< media::TextToSpeechPlayer >& textToSpeechPlayer, const std::shared_ptr< media::ImagePlayer >& imagePlayer,
			const std::shared_ptr< media::TextPlayer >& textPlayer, QWidget* parent = nullptr);

		/**
		 * @brief Default Destructor
		 */
		virtual ~QuizEntry() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		QuizEntry(const QuizEntry&) = delete;
		QuizEntry& operator=(const QuizEntry&) = delete;

		enum class EntryState {
			IDLE = 1, // Default
			PLAYING = 2,
			PAUSED = 3,
			PLAYING_ANSWER = 4,
			PLAYED = 5,
		};

		enum class EntryType {
			Song = 0, Video = 1, TextToSpeech = 2, Image = 3, Text = 4
		};

		/**
		 * @brief Get the entry state.
		 *
		 * @return The entry state.
		 */
		EntryState getEntryState();

	public slots:
		/**
		 * @brief Sets the color of the button (used after the entry is answered).
		 *
		 * @param[in] color The color.
		 */
		void setColor(const QColor& color);

		/**
		 * @brief Enables / disables the hidden answers setting.
		 *
		 * @param[in] hidden If true the answers will be hidden.
		 */
		void setHiddenAnswer(bool hidden);

		/**
		 * @brief Enables / disables double points.
		 *
		 * @param[in] enabled If true the entry will give double points.
		 * @param[in] hidden If true the entry will not have a border around showing that it is double points.
		 */
		void setDoublePointsEnabled(bool enabled, bool hidden = true);

		/**
		 * @brief Enables / disables triple points.
		 *
		 * @param[in] enabled If true the entry will give triple points.
		 * @param[in] hidden If true the entry will not have a border around showing that it is triple points.
		 */
		void setTriplePointsEnabled(bool enabled, bool hidden = true);

	signals:
		void answered(size_t points);
		void played();
		void blurQuiz();
		void unBlurQuiz();
		void startCountdown();
		void stopCountdown();

	protected:
		/**
		 * @brief Override the mouse release event.
		 *
		 * @param[in] event The event.
		 */
		void mouseReleaseEvent(QMouseEvent* event);

		/**
		 * @brief Handles the mouse release event.
		 *
		 * @param[in] event The event.
		 */
		void handleMouseEvent(QMouseEvent* event);

		/**
		 * @brief Handles the mouse left click event.
		 */
		void leftClickEvent();

		/**
		 * @brief Handles the mouse right click event.
		 */
		void rightClickEvent();

		/**
		 * @brief Applys the color to the button.
		 *
		 * @param[in] color The color.
		 */
		void applyColor(const QColor& color);

		/** Variables */
		size_t _points = 0;
		size_t _fontSize = 40;
		bool _textSizeSet = false;

		size_t _startTime = 0;
		size_t _videoStartTime = 0;
		size_t _answerStartTime = 0;

		std::filesystem::path _audioFile = "";
		std::filesystem::path _videoFile = "";
		std::filesystem::path _imageFile = "";
		QString _speechString = "";
		QString _textString = "";

		QString _answer = "";
		bool _entryAnswered = false;
		QColor _answeredColor = QColor(0, 0, 120);

		EntryType _type = EntryType::Song;
		EntryState _state = EntryState::IDLE;

		std::shared_ptr< media::AudioPlayer > _audioPlayer = nullptr;
		std::shared_ptr< media::VideoPlayer > _videoPlayer = nullptr;
		std::shared_ptr< media::TextToSpeechPlayer > _textToSpeechPlayer = nullptr;
		const media::TextToSpeechPlayer::TextToSpeechSettings _textToSpeechSettings;
		std::shared_ptr< media::ImagePlayer > _imagePlayer = nullptr;
		std::shared_ptr< media::TextPlayer > _textPlayer = nullptr;

		std::function< void(QMouseEvent*) > _mouseEventCallback;

		/** Settings */
		bool _hiddenAnswer = false;
		bool _doublePoints = false;
		bool _triplePoints = false;
		bool _hiddenDoublePoints = false;
		bool _hiddenTriplePoints = false;
	};
}