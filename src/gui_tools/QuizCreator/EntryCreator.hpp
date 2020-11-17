#pragma once 

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

namespace common {
	class Configuration;
}


namespace MusicQuiz {

	class EntryCreator : public QWidget {
		Q_OBJECT

	public:
		enum class EntryType {
			Song = 0, Video = 1
		};

		/**
		 * @brief Constructor
		 *
		 * @param[in] name The entry name.
		 * @param[in] points The points of the entry.
		 * @param[in] audioPlayer The audio player.
		 * @param[in] config configuration.
		 * @param[in] parent The parent widget.
		 */
		explicit EntryCreator(const QString& name, int points, const std::shared_ptr< media::AudioPlayer >& audioPlayer, const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Constructor from boost property tree
		 *
		 * @param[in] tree the tree which the entry should be loaded from.
		 * @param[in] audioPlayer The audio player.
		 * @param[in] config configuration.
		 * @param[in] parent The parent widget.
		 */

		explicit EntryCreator(const boost::property_tree::ptree &tree, const media::AudioPlayer::Ptr& audioPlayer, const common::Configuration& config, QWidget* parent = nullptr);

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
		 * @brief Sets the entry type [Song, Video].
		 *
		 * @param[in] type The entry type.
		 */
		void setType(const EntryType& type);

		/**
		 * @brief Gets the entry type [Song, Video].
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
		 * @brief serialize song media into the boost property_tree
		 *
		 * @param[out] tree tree to serialize into.
		 * @param[in] savePath path where the mediafiles should be saved.
		 * @param[in] xmlPath path for the media files that should be written in the ptree.
		 * 
		 * @return the serialized ptree
		 */
		void saveSongToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const;

		/**
		 * @brief serialize video media into the boost property_tree
		 *
		 * @param[out] tree tree to serialize into.
		 * @param[in] savePath path where the mediafiles should be saved.
		 * @param[in] xmlPath path for the media files that should be written in the ptree.
		 * 
		 * @return the serialized ptree
		 */
		void saveVideoToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const;


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
		 * @brief Check if the song file name is valid and enables / disables widgets.
		 */
		void checkSongFileName();

		/**
		 * @brief Check if the video file names is valid and enables / disables widgets.
		 */
		void checkVideoFiles();

		/**
		 * @brief Plays the song file from the start position defined in the start QTimeEdit.
		 */
		void playSong();

		/**
		 * @brief Pauses the audio and video playing.
		 */
		void pause();

		/**
		 * @brief Plays the video file from the start position defined in the start QTimeEdit.
		 */
		void playVideo();

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
		QGridLayout* createSongFileLayout();

		/**
		 * @brief Creates the video file category layout.
		 */
		QGridLayout* createVideoFileLayout();

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
		 * @brief load song media from boost property_tree
		 *
		 * @param[out] tree tree to load from.
		 */
		void loadSongFromXml(const boost::property_tree::ptree &tree);

		/**
		 * @brief load video media from boost property_tree
		 *
		 * @param[out] tree tree to load from.
		 */
		void loadVideoFromXml(const boost::property_tree::ptree &tree);

		/** Variables */
		int _points = 0;

		QString _entryName;
		QLabel* _entryNameLabel = nullptr;

		QButtonGroup* _buttonGroup = nullptr;
		EntryType _entryType = EntryType::Song;

		QSpinBox* _pointsSpinbox = nullptr;

		QLineEdit* _songFileLineEdit = nullptr;
		QLineEdit* _videoFileLineEdit = nullptr;
		QLineEdit* _videoSongFileLineEdit = nullptr;

		QTimeEdit* _songStartTimeEdit = nullptr;
		QTimeEdit* _answerStartTimeEdit = nullptr;

		QTimeEdit* _videoStartTimeEdit = nullptr;
		QTimeEdit* _videoSongStartTimeEdit = nullptr;
		QTimeEdit* _videoAnswerStartTimeEdit = nullptr;

		QWidget* _songSettings = nullptr;
		QWidget* _videoSettings = nullptr;
		QPushButton* _browseSongBtn = nullptr;
		QPushButton* _browseVideoBtn = nullptr;
		QPushButton* _browseVideoSongBtn = nullptr;

		std::shared_ptr< media::AudioPlayer > _audioPlayer = nullptr;
		media::VideoPlayer* _videoPlayer = nullptr;

		const std::vector< QString > _validAudioFormats = { ".mp3", ".mp4", ".wav" };
		const std::vector< QString > _validVideoFormats = { ".mp4" };

		const common::Configuration& _config;
	};
}