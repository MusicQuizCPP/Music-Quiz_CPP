#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QSpinBox>
#include <QTimeEdit>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QTableWidget>

#include <boost/property_tree/ptree.hpp>

#include "media/AudioPlayer.hpp"
#include "media/VideoPlayer.hpp"
#include "media/TextToSpeechPlayer.hpp"


namespace common {
	class Configuration;
}

namespace gui_tools {
	namespace GuiUtil {
		class QSliderWidget;
	}
}

namespace MusicQuiz {

	class EntryCreator : public QWidget {
		Q_OBJECT

	public:
		enum class EntryType {
			Song = 0, Video = 1, TextToSpeech = 2, Image = 3, Text = 4
		};

		/**
		 * @brief Constructor
		 *
		 * @param[in] name               The entry name.
		 * @param[in] points             The points of the entry.
		 * @param[in] audioPlayer        The audio player.
		 * @param[in] textToSpeechPlayer The text to speech.
		 * @param[in] config             The configuration.
		 * @param[in] parent             The parent widget.
		 */
		explicit EntryCreator(const QString& name, int points, const std::shared_ptr< media::AudioPlayer >& audioPlayer,
			const std::shared_ptr< media::TextToSpeechPlayer >& textToSpeechPlayer, const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Constructor from boost property tree
		 *
		 * @param[in] tree               The tree which the entry should be loaded from.
		 * @param[in] audioPlayer        The audio player.
		 * @param[in] textToSpeechPlayer The text to speech.
		 * @param[in] config             The configuration.
		 * @param[in] parent             The parent widget.
		 */
		explicit EntryCreator(const boost::property_tree::ptree &tree, const media::AudioPlayer::Ptr& audioPlayer,
			const std::shared_ptr< media::TextToSpeechPlayer >& textToSpeechPlayer, const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Default destructor
		 */
		virtual ~EntryCreator() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		EntryCreator(const EntryCreator&) = delete;
		EntryCreator& operator=(const EntryCreator&) = delete;

	public slots:
		/**
		 * @brief Updates the entry name.
		 *
		 * @param[in] name The name.
		 */
		void setName(const QString& name);

		/**
		 * @brief Gets the entry name.
		 *
		 * @return The name.
		 */
		const QString getName() const;

		/**
		 * @brief Sets the points.
		 *
		 * @param[in] points The points.
		 */
		void setPoints(int points);

		/**
		 * @brief Gets the points.
		 *
		 * @return The points.
		 */
		size_t getPoints() const;

		/**
		 * @brief Sets the entry type [Song, Video, Text to Speech, Image, Text].
		 *
		 * @param[in] type The entry type.
		 */
		void setType(const EntryType& type);

		/**
		 * @brief Gets the entry type [Song, Video, Text to Speech, Image, Text].
		 *
		 * @return The entry type.
		 */
		EntryType getType() const;

		/**
		 * @brief Sets the song file.
		 *
		 * @param[in] file The song file.
		 */
		void setSongFile(const QString& file);

		/**
		 * @brief Gets the song file.
		 *
		 * @return The song file.
		 */
		const QString getSongFile() const;

		/**
		 * @brief Sets the video file.
		 *
		 * @param[in] file The video file.
		 */
		void setVideoFile(const QString& file);

		/**
		 * @brief Gets the video file.
		 *
		 * @return The video file.
		 */
		const QString getVideoFile() const;

		/**
		 * @brief Sets the video song file.
		 *
		 * @param[in] file The video song file.
		 */
		void setVideoSongFile(const QString& file);

		/**
		 * @brief Gets the video song file.
		 *
		 * @return The video song file.
		 */
		const QString getVideoSongFile() const;

		/**
		 * @brief Sets the text to speech answer song file.
		 *
		 * @param[in] file The text to speech answer song file.
		 */
		void setTextToSpeechAnswerSongFile(const QString& file);

		/**
		 * @brief Gets the text to speech answer song file.
		 *
		 * @return The text to speech answer song file.
		 */
		const QString getTextToSpeechAnswerSongFile() const;

		/**
		 * @brief Sets the image file.
		 *
		 * @param[in] The image file.
		 */
		void setImageFile(const QString& file);

		/**
		 * @brief Gets the image file.
		 *
		 * @return The image file.
		 */
		const QString getImageFile() const;

		/**
		 * @brief Sets the image answer song file.
		 *
		 * @param[in] The image answer song file.
		 */
		void setImageAnswerSongFile(const QString& file);

		/**
		 * @brief Gets the image answer song file.
		 *
		 * @return The image answer song file.
		 */
		const QString getImageAnswerSongFile() const;

		/**
		 * @brief Sets the text answer song file.
		 *
		 * @param[in] file The text answer song file.
		 */
		void setTextAnswerSongFile(const QString& file);

		/**
		 * @brief Gets the text answer song file.
		 *
		 * @return The text answer song file.
		 */
		const QString getTextAnswerSongFile() const;

		/**
		 * @brief Sets the song start time.
		 *
		 * @param[in] time The song start time.
		 */
		void setSongStartTime(size_t time);

		/**
		 * @brief Gets the song start time.
		 *
		 * @return The song start time.
		 */
		size_t getSongStartTime() const;

		/**
		 * @brief Sets the answer start time.
		 *
		 * @param[in] time The answer start time.
		 */
		void setAnswerStartTime(size_t time);

		/**
		 * @brief Gets the answer start time.
		 *
		 * @return The answer start time.
		 */
		size_t getAnswerStartTime() const;

		/**
		 * @brief Sets the video start time.
		 *
		 * @param[in] time The video start time.
		 */
		void setVideoStartTime(size_t time);

		/**
		 * @brief Gets the video start time.
		 *
		 * @return The video start time.
		 */
		size_t getVideoStartTime() const;

		/**
		 * @brief Sets the video song start time.
		 *
		 * @param[in] time The video song start time.
		 */
		void setVideoSongStartTime(size_t time);

		/**
		 * @brief Gets the video song start time.
		 *
		 * @return The video song start time.
		 */
		size_t getVideoSongStartTime() const;

		/**
		 * @brief Sets the video answer start time.
		 *
		 * @param[in] time The video answer start time.
		 */
		void setVideoAnswerStartTime(size_t time);

		/**
		 * @brief Gets the video answer start time.
		 *
		 * @return The video answer start time.
		 */
		size_t getVideoAnswerStartTime() const;

		/**
		 * @brief Sets the text to speech string.
		 *
		 * @param[in] textToSpeechString The text to speech string.
		 */
		void setTextToSpeechString(const QString& textToSpeechString);

		/**
		 * @brief Gets the text to speech string.
		 *
		 * @return The text to speech string.
		 */
		QString getTextToSpeechString() const;

		/**
		 * @brief Sets the text to speech answer start time.
		 *
		 * @param[in] time The text to speech answer start time.
		 */
		void setTextToSpeechAnswerStartTime(size_t time);

		/**
		 * @brief Gets the text to speech answer start time.
		 *
		 * @return The text to speech answer start time.
		 */
		size_t getTextToSpeechAnswerStartTime() const;

		/**
		 * @brief Sets the pitch.
		 *
		 * @param[in] pitch The pitch.
		 */
		void setPitch(double pitch);

		/**
		 * @brief Gets the pitch.
		 *
		 * @return The pitch.
		 */
		double getPitch() const;

		/**
		 * @brief Sets the text to speech rate.
		 *
		 * @param[in] rate The text to speech rate.
		 */
		void setRate(double pitch);

		/**
		 * @brief Gets the text to speech rate.
		 *
		 * @return  The text to speech rate.
		 */
		double getRate() const;

		/**
		 * @brief Sets the voice.
		 *
		 * @param[in] voiceName The name of the voice to set.
		 */
		void setVoice(const QString& voiceName);

		/**
		 * @brief Gets the name of the voice.
		 *
		 * @return The name of the set voice.
		 */
		QString getVoiceName() const;

		/**
		 * @brief Sets the image answer start time.
		 *
		 * @param[in] time The image answer start time.
		 */
		void setImageAnswerStartTime(size_t time);

		/**
		 * @brief Gets the image answer start time.
		 *
		 * @return The image answer start time.
		 */
		size_t getImageAnswerStartTime() const;

		/**
		 * @brief Sets the text string.
		 *
		 * @param[in] textString The text string.
		 */
		void setTextString(const QString& textString);

		/**
		 * @brief Gets the text string.
		 *
		 * @return The text string.
		 */
		QString getTextString() const;

		/**
		 * @brief Sets the text answer start time.
		 *
		 * @param[in] time The text answer start time.
		 */
		void setTextAnswerStartTime(size_t time);

		/**
		 * @brief Gets the text answer start time.
		 *
		 * @return The text answer start time.
		 */
		size_t getTextAnswerStartTime() const;

		/**
		 * @brief Stops the audio and video playing.
		 */
		void stop();

		/**
		 * @brief serialize entry into a boost property_tree
		 *
		 * @param[in] savePath path where the mediafiles should be saved.
		 * @param[in] xmlPath path for the media files that should be written in the ptree.
		 * 
		 * @return the serialized ptree
		 */

		boost::property_tree::ptree toXml(const std::string& savePath, const std::string xmlPath) const;

		/**
		 * @brief Serialize song media into the boost property_tree
		 *
		 * @param[out] tree    tree to serialize into.
		 * @param[in] savePath path where the media files should be saved.
		 * @param[in] xmlPath  path for the media files that should be written in the ptree.
		 */
		void saveSongToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const;

		/**
		 * @brief Serialize video media into the boost property_tree
		 *
		 * @param[out] tree    tree to serialize into.
		 * @param[in] savePath path where the media files should be saved.
		 * @param[in] xmlPath  path for the media files that should be written in the ptree.
		 */
		void saveVideoToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const;

		/**
		 * @brief Serialize text to speech media into the boost property_tree
		 *
		 * @param[out] tree    tree to serialize into.
		 * @param[in] savePath path where the media files should be saved.
		 * @param[in] xmlPath  path for the media files that should be written in the ptree.
		 */
		void saveTextToSpeechToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const;

		/**
		 * @brief Serialize image media into the boost property_tree
		 *
		 * @param[out] tree    tree to serialize into.
		 * @param[in] savePath path where the media files should be saved.
		 * @param[in] xmlPath  path for the media files that should be written in the ptree.
		 */
		void saveImageToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const;

		/**
		 * @brief Serialize text media into the boost property_tree
		 *
		 * @param[out] tree    tree to serialize into.
		 * @param[in] savePath path where the media files should be saved.
		 * @param[in] xmlPath  path for the media files that should be written in the ptree.
		 */
		void saveTextToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const;

	private slots:
		/**
		 * @brief Opens a dialog to browse for a song file.
		 */
		void browseSong();

		/**
		 * @brief Opens a dialog to browse for a video file.
		 */
		void browseVideo();

		/**
		 * @brief Opens a dialog to browse for a song file.
		 */
		void browseVideoSong();

		/**
		 * @brief Opens a dialog to browse for a song file.
		 */
		void browseTextToSpeechAnswerSong();

		/**
		 * @brief Opens a dialog to browse for a image file.
		 */
		void browseImage();

		/**
		 * @brief Opens a dialog to browse for a song file.
		 */
		void browseImageAnswerSong();

		/**
		 * @brief Opens a dialog to browse for a song file.
		 */
		void browseTextAnswerSong();

		/**
		 * @brief Check if the song file name is valid and enables / disables widgets.
		 */
		void checkSongFileName();

		/**
		 * @brief Check if the video file names is valid and enables / disables widgets.
		 */
		void checkVideoFiles();

		/**
		 * @brief Check if the song file name is valid and enables / disables widgets.
		 */
		void checkTextToSpeechAnswerSongFileName();

		/**
		 * @brief Check if the text to speech lyrics are valid and enables / disables widgets.
		 */
		void checkTextToSpeechLyrics();

		/**
		 * @brief Check if the image file names is valid and enables / disables widgets.
		 */
		void checkImageFiles();

		/**
		 * @brief Check if the song file name is valid and enables / disables widgets.
		 */
		void checkTextAnswerSongFileName();

		/**
		 * @brief Plays the song file from the start position defined in the start QTimeEdit.
		 */
		void playSong();

		/**
		 * @brief Plays the video file from the start position defined in the start QTimeEdit.
		 */
		void playVideo();

		/**
		 * @brief Plays the text to speech.
		 */
		void playText();

		/**
		 * @brief Pauses the audio and video playing.
		 */
		void pause();

		/**
		 * @brief Sets the entry type [0 = song, 1 = video].
		 *
		 * @param[in] index The type index.
		 */
		void setEntryType(int index);

		/**
		 * @brief Updates the points.
		 *
		 * @param[in] points The points.
		 */
		void pointsChanged(int points);

	private:
		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/**
		 * @brief Creates the song file category layout.
		 */
		QGridLayout* createSongLayout();

		/**
		 * @brief Creates the video file category layout.
		 */
		QGridLayout* createVideoLayout();

		/**
		 * @brief Creates the text to speech category layout.
		 */
		QGridLayout* createTextToSpeechLayout();

		/**
		 * @brief Creates the image category layout.
		 */
		QGridLayout* createImageLayout();

		/**
		 * @brief Creates the text category layout.
		 */
		QGridLayout* createTextLayout();

		/**
		 * @brief Checks if the song file name is valid.
		 *
		 * @param[in] fileName The file name.
		 *
		 * @return True is name is valid.
		 */
		bool isSongFileValid(const QString& fileName) const;

		/**
		 * @brief Checks if the video file name is valid.
		 *
		 * @param[in] fileName The file name.
		 *
		 * @return True is name is valid.
		 */
		bool isVideoFileValid(const QString& fileName) const;

		/**
		 * @brief Checks if the image file name is valid.
		 *
		 * @param[in] fileName The file name.
		 *
		 * @return True is name is valid.
		 */
		bool isImageFileValid( const QString& fileName ) const;

		/**
		 * @brief Gets the time in msec from a QTime.
		 * @note The Qtime is used so the hh::mm corresponds to mm::ss.
		 *
		 * @param[in] time The time.
		 *
		 * @return The time in miliseconds.
		 */
		size_t toMSec(const QTime& time) const;

		/**
		 * @brief Gets the time in QTime from msec.
		 * @note The Qtime is used so the hh::mm corresponds to mm::ss.
		 *
		 * @param[in] time The time.
		 *
		 * @return The QTime.
		 */
		QTime fromMSec(size_t time) const;

		/**
		 * @brief Load song media from boost property_tree
		 *
		 * @param[out] tree The tree to load from.
		 */
		void loadSongFromXml(const boost::property_tree::ptree &tree);

		/**
		 * @brief Load video media from boost property_tree
		 *
		 * @param[out] tree The tree to load from.
		 */
		void loadVideoFromXml(const boost::property_tree::ptree &tree);

		/**
		 * @brief Load text to speech media from boost property_tree
		 *
		 * @param[out] tree The tree to load from.
		 */
		void loadTextToSpeechFromXml(const boost::property_tree::ptree& tree);

		/**
		 * @brief Load image media from boost property_tree
		 *
		 * @param[out] tree The tree to load from.
		 */
		void loadImageFromXml(const boost::property_tree::ptree& tree);

		/**
		 * @brief Load text media from boost property_tree
		 *
		 * @param[out] tree The tree to load from.
		 */
		void loadTextFromXml(const boost::property_tree::ptree& tree);

		/** Variables */
		int _points = 0;

		QString _entryName;
		QLabel* _entryNameLabel = nullptr;

		QButtonGroup* _buttonGroup = nullptr;
		EntryType _entryType = EntryType::Song;

		QSpinBox* _pointsSpinbox = nullptr;

		QWidget* _songLayout = nullptr;
		QWidget* _videoLayout = nullptr;
		QWidget* _textToSpeechLayout = nullptr;
		QWidget* _imageLayout = nullptr;
		QWidget* _textLayout = nullptr;

		QLineEdit* _songFileLineEdit = nullptr;
		QLineEdit* _videoFileLineEdit = nullptr;
		QLineEdit* _videoSongFileLineEdit = nullptr;
		QLineEdit* _textToSpeechAnswerSongFileLineEdit = nullptr;
		QLineEdit* _imageFileLineEdit = nullptr;
		QLineEdit* _imageAnswerSongFileLineEdit = nullptr;
		QLineEdit* _textAnswerSongFileLineEdit = nullptr;

		QTimeEdit* _songStartTimeEdit = nullptr;
		QTimeEdit* _answerStartTimeEdit = nullptr;
		QTimeEdit* _textToSpeechAnswerStartTimeEdit = nullptr;
		QTimeEdit* _imageAnswerStartTimeEdit = nullptr;
		QTimeEdit* _textAnswerStartTimeEdit = nullptr;

		QTimeEdit* _videoStartTimeEdit = nullptr;
		QTimeEdit* _videoSongStartTimeEdit = nullptr;
		QTimeEdit* _videoAnswerStartTimeEdit = nullptr;

		QTextEdit* _textToSpeechTextEdit = nullptr;
		gui_tools::GuiUtil::QSliderWidget* _pitchSlider = nullptr;
		gui_tools::GuiUtil::QSliderWidget* _rateSlider = nullptr;
		QButtonGroup* _voiceButtonGroup = nullptr;

		QLabel* _imagePreviewLabel = nullptr;

		QTextEdit* _textTextEdit = nullptr;

		QWidget* _songSettings = nullptr;
		QWidget* _videoSettings = nullptr;
		QWidget* _textToSpeechSettings = nullptr;
		QWidget* _textToSpeechAnswerSettings = nullptr;
		QWidget* _imageAnswerSettings = nullptr;
		QWidget* _textAnswerSettings = nullptr;
		QPushButton* _browseSongBtn = nullptr;
		QPushButton* _browseVideoBtn = nullptr;
		QPushButton* _browseVideoSongBtn = nullptr;
		QPushButton* _browseTextToSpeechAnswerSongBtn = nullptr;
		QPushButton* _browseImageBtn = nullptr;
		QPushButton* _browseImageAnswerSongBtn = nullptr;
		QPushButton* _browseTextAnswerSongBtn = nullptr;

		std::shared_ptr< media::AudioPlayer > _audioPlayer = nullptr;
		media::VideoPlayer* _videoPlayer = nullptr;
		std::shared_ptr< media::TextToSpeechPlayer > _textToSpeechPlayer = nullptr;

		const std::vector< QString > _validAudioFormats = { ".mp3", ".mp4", ".wav" };
		const std::vector< QString > _validVideoFormats = { ".mp4" };
		const std::vector< QString > _validImageFormats = { ".bmp", ".jpg", ".jpeg", ".png", ".ppm" };

		const common::Configuration& _config;
	};
}