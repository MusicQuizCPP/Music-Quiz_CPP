#include "EntryCreator.hpp"

#include <filesystem>

#include <QTime>
#include <QLabel>
#include <QString>
#include <QScrollArea>
#include <QFileDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QMediaContent>

#include "common/Configuration.hpp"

using namespace std;

MusicQuiz::EntryCreator::EntryCreator(const QString& name, const int points, const media::AudioPlayer::Ptr& audioPlayer, const common::Configuration& config, QWidget* parent) :
	QWidget(parent), _points(points), _entryName(name), _audioPlayer(audioPlayer), _config(config)
{
	/** Create Layout */
	createLayout();
}

void MusicQuiz::EntryCreator::createLayout()
{
	/** Scroll Area */
	QGridLayout* scrollLayout = new QGridLayout;
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidgetResizable(true);
	QWidget* scrollWidget = new QWidget;

	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	mainlayout->setHorizontalSpacing(10);
	mainlayout->setVerticalSpacing(10);
	mainlayout->setColumnStretch(0, 1);
	mainlayout->setColumnStretch(1, 3);
	int row = 0;

	/** Entry Name */
	_entryNameLabel = new QLabel(_entryName);
	_entryNameLabel->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(_entryNameLabel, ++row, 0, 1, 2, Qt::AlignCenter);

	/** Points */
	QLabel* label = new QLabel("Points:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	_pointsSpinbox = new QSpinBox;
	_pointsSpinbox->setAlignment(Qt::AlignCenter);
	_pointsSpinbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_pointsSpinbox->setObjectName("quizCreatorSpinbox");
	_pointsSpinbox->setRange(0, 10000);
	_pointsSpinbox->setSingleStep(100);
	_pointsSpinbox->setValue(_points);
	connect(_pointsSpinbox, SIGNAL(valueChanged(int)), this, SLOT(pointsChanged(int)));
	mainlayout->addWidget(_pointsSpinbox, row, 1);

	/** Type */
	QHBoxLayout* typeLayout = new QHBoxLayout;
	typeLayout->setStretch(0, 1);
	typeLayout->setStretch(1, 1);
	typeLayout->setStretch(2, 1);
	label = new QLabel("Type:");
	label->setObjectName("quizCreatorLabel");
	typeLayout->addWidget(label);

	_buttonGroup = new QButtonGroup;
	QRadioButton* songBtn = new QRadioButton("Song");
	QRadioButton* videoBtn = new QRadioButton("Video");
	songBtn->setObjectName("quizCreatorRadioButton");
	videoBtn->setObjectName("quizCreatorRadioButton");
	_buttonGroup->addButton(songBtn, 0);
	_buttonGroup->addButton(videoBtn, 1);
	connect(_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setEntryType(int)));
	songBtn->setChecked(true);

	typeLayout->addWidget(songBtn);
	typeLayout->addWidget(videoBtn);
	mainlayout->addItem(typeLayout, ++row, 0, 1, 2);

	/** Song File Layout */
	mainlayout->addItem(createSongFileLayout(), ++row, 0, 1, 2);

	/** Video - File */
	mainlayout->addItem(createVideoFileLayout(), ++row, 0, 1, 2);

	/** Set Type to song */
	setEntryType(0);

	/** Set Layout */
	mainlayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Expanding), ++row, 0, 1, 2);

	/** Set Layout */
	scrollWidget->setLayout(mainlayout);
	scrollArea->setWidget(scrollWidget);
	scrollLayout->addWidget(scrollArea);
	setLayout(scrollLayout);
}

QGridLayout* MusicQuiz::EntryCreator::createSongFileLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QHBoxLayout* songFileLayout = new QHBoxLayout;
	QGridLayout* songSettingsLayout = new QGridLayout;
	songSettingsLayout->setHorizontalSpacing(5);
	songSettingsLayout->setVerticalSpacing(10);
	mainlayout->setVerticalSpacing(10);
	int row = 0;

	/** Song - File */
	QLabel* label = new QLabel("Song File:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	songFileLayout->setSpacing(10);
	_songFileLineEdit = new QLineEdit;
	_songFileLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_songFileLineEdit->setObjectName("quizCreatorLineEdit");
	connect(_songFileLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkSongFileName()));
	songFileLayout->addWidget(_songFileLineEdit);

	_browseSongBtn = new QPushButton;
	_browseSongBtn->setObjectName("quizCreatorBrowseBtn");
	connect(_browseSongBtn, SIGNAL(released()), this, SLOT(browseSong()));
	songFileLayout->addWidget(_browseSongBtn);
	mainlayout->addItem(songFileLayout, ++row, 0, 1, 2);

	/** Song - Set Song Start */
	label = new QLabel("Song:");
	label->setObjectName("quizCreatorLabel");
	songSettingsLayout->addWidget(label, 0, 0, 1, 1);

	/** Song Start Time */
	_songStartTimeEdit = new QTimeEdit;
	_songStartTimeEdit->setAlignment(Qt::AlignCenter);
	_songStartTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_songStartTimeEdit->setObjectName("quizCreatorTimeEdit");
	_songStartTimeEdit->setDisplayFormat("mm:ss");
	songSettingsLayout->addWidget(_songStartTimeEdit, 0, 1, 1, 1);

	/** Song Audio Buttons - Play */
	QPushButton* btn = new QPushButton;
	btn->setProperty("type", "song");
	btn->setObjectName("quizCreatorPlayBtn");
	connect(btn, SIGNAL(released()), this, SLOT(playSong()));
	songSettingsLayout->addWidget(btn, 0, 2, 1, 1);

	/** Song Audio Buttons - Pause */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorPauseBtn");
	connect(btn, SIGNAL(released()), this, SLOT(pause()));
	songSettingsLayout->addWidget(btn, 0, 3, 1, 1);

	/** Song Audio Buttons - Stop */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorStopBtn");
	connect(btn, SIGNAL(released()), this, SLOT(stop()));
	songSettingsLayout->addWidget(btn, 0, 4, 1, 1);

	/** Song - Set Answer Start */
	label = new QLabel("Answer:");
	label->setObjectName("quizCreatorLabel");
	songSettingsLayout->addWidget(label, 1, 0, 1, 1);

	/** Answer Start Time */
	_answerStartTimeEdit = new QTimeEdit;
	_answerStartTimeEdit->setAlignment(Qt::AlignCenter);
	_answerStartTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_answerStartTimeEdit->setObjectName("quizCreatorTimeEdit");
	_answerStartTimeEdit->setDisplayFormat("mm:ss");
	songSettingsLayout->addWidget(_answerStartTimeEdit, 1, 1, 1, 1);

	/** Answer Audio Buttons - Play */
	btn = new QPushButton;
	btn->setProperty("type", "songAnswer");
	btn->setObjectName("quizCreatorPlayBtn");
	connect(btn, SIGNAL(released()), this, SLOT(playSong()));
	songSettingsLayout->addWidget(btn, 1, 2, 1, 1);

	/** Answer Audio Buttons - Pause */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorPauseBtn");
	connect(btn, SIGNAL(released()), this, SLOT(pause()));
	songSettingsLayout->addWidget(btn, 1, 3, 1, 1);

	/** Answer Audio Buttons - Stop */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorStopBtn");
	connect(btn, SIGNAL(released()), this, SLOT(stop()));
	songSettingsLayout->addWidget(btn, 1, 4, 1, 1);

	/** Add layout to settings widget */
	_songSettings = new QWidget;
	_songSettings->setEnabled(false);
	_songSettings->setLayout(songSettingsLayout);
	mainlayout->addWidget(_songSettings, ++row, 0, 1, 2);

	/** Set Layout */
	return mainlayout;
}

QGridLayout* MusicQuiz::EntryCreator::createVideoFileLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QHBoxLayout* videoLayout = new QHBoxLayout;
	QHBoxLayout* videoFileLayout = new QHBoxLayout;
	QHBoxLayout* videoSongFileLayout = new QHBoxLayout;
	QGridLayout* videoSettingsLayout = new QGridLayout;
	videoSettingsLayout->setHorizontalSpacing(5);
	videoSettingsLayout->setVerticalSpacing(10);
	mainlayout->setVerticalSpacing(10);
	int row = 0;

	/** Video - File */
	QLabel* label = new QLabel("Video File:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	videoFileLayout->setSpacing(10);
	_videoFileLineEdit = new QLineEdit;
	_videoFileLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_videoFileLineEdit->setObjectName("quizCreatorLineEdit");
	connect(_videoFileLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkVideoFiles()));
	videoFileLayout->addWidget(_videoFileLineEdit);

	_browseVideoBtn = new QPushButton;
	_browseVideoBtn->setObjectName("quizCreatorBrowseBtn");
	connect(_browseVideoBtn, SIGNAL(released()), this, SLOT(browseVideo()));
	videoFileLayout->addWidget(_browseVideoBtn);
	mainlayout->addItem(videoFileLayout, ++row, 0, 1, 2);

	/** Video - Song File */
	label = new QLabel("Video Song File:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	videoSongFileLayout->setSpacing(10);
	_videoSongFileLineEdit = new QLineEdit;
	_videoSongFileLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_videoSongFileLineEdit->setObjectName("quizCreatorLineEdit");
	connect(_videoSongFileLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkVideoFiles()));
	videoSongFileLayout->addWidget(_videoSongFileLineEdit);

	_browseVideoSongBtn = new QPushButton;
	_browseVideoSongBtn->setObjectName("quizCreatorBrowseBtn");
	connect(_browseVideoSongBtn, SIGNAL(released()), this, SLOT(browseVideoSong()));
	videoSongFileLayout->addWidget(_browseVideoSongBtn);
	mainlayout->addItem(videoSongFileLayout, ++row, 0, 1, 2);

	/** Video Widget */
	_videoPlayer = new media::VideoPlayer(this);
	videoLayout->addWidget(_videoPlayer);
	videoLayout->setAlignment(_videoPlayer, Qt::AlignCenter);
	mainlayout->addItem(videoLayout, ++row, 0, 1, 2);

	/** Video - Set Video Start */
	label = new QLabel("Video:");
	label->setObjectName("quizCreatorLabel");
	videoSettingsLayout->addWidget(label, 1, 0, 1, 1);

	/** Video Start Time */
	_videoStartTimeEdit = new QTimeEdit;
	_videoStartTimeEdit->setAlignment(Qt::AlignCenter);
	_videoStartTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_videoStartTimeEdit->setObjectName("quizCreatorTimeEdit");
	_videoStartTimeEdit->setDisplayFormat("mm:ss");
	videoSettingsLayout->addWidget(_videoStartTimeEdit, 1, 1, 1, 1);

	/** Video Buttons - Play */
	QPushButton* btn = new QPushButton;
	btn->setProperty("type", "video");
	btn->setObjectName("quizCreatorPlayBtn");
	connect(btn, SIGNAL(released()), this, SLOT(playVideo()));
	videoSettingsLayout->addWidget(btn, 1, 2, 1, 1);

	/** Video Buttons - Pause */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorPauseBtn");
	connect(btn, SIGNAL(released()), this, SLOT(pause()));
	videoSettingsLayout->addWidget(btn, 1, 3, 1, 1);

	/** Video Buttons - Stop */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorStopBtn");
	connect(btn, SIGNAL(released()), this, SLOT(stop()));
	videoSettingsLayout->addWidget(btn, 1, 4, 1, 1);

	/** Video - Set Answer Start */
	label = new QLabel("Answer:");
	label->setObjectName("quizCreatorLabel");
	videoSettingsLayout->addWidget(label, 2, 0, 1, 1);

	/** Answer Start Time */
	_videoAnswerStartTimeEdit = new QTimeEdit;
	_videoAnswerStartTimeEdit->setAlignment(Qt::AlignCenter);
	_videoAnswerStartTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_videoAnswerStartTimeEdit->setObjectName("quizCreatorTimeEdit");
	_videoAnswerStartTimeEdit->setDisplayFormat("mm:ss");
	videoSettingsLayout->addWidget(_videoAnswerStartTimeEdit, 2, 1, 1, 1);

	/** Answer Video Buttons - Play */
	btn = new QPushButton;
	btn->setProperty("type", "videoAnswer");
	btn->setObjectName("quizCreatorPlayBtn");
	connect(btn, SIGNAL(released()), this, SLOT(playVideo()));
	videoSettingsLayout->addWidget(btn, 2, 2, 1, 1);

	/** Answer Video Buttons - Pause */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorPauseBtn");
	connect(btn, SIGNAL(released()), this, SLOT(pause()));
	videoSettingsLayout->addWidget(btn, 2, 3, 1, 1);

	/** Answer Video Buttons - Stop */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorStopBtn");
	connect(btn, SIGNAL(released()), this, SLOT(stop()));
	videoSettingsLayout->addWidget(btn, 2, 4, 1, 1);

	/** Video Song - Set Video Start */
	label = new QLabel("Song:");
	label->setObjectName("quizCreatorLabel");
	videoSettingsLayout->addWidget(label, 3, 0, 1, 1);

	/** Video Song Start Time */
	_videoSongStartTimeEdit = new QTimeEdit;
	_videoSongStartTimeEdit->setAlignment(Qt::AlignCenter);
	_videoSongStartTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_videoSongStartTimeEdit->setObjectName("quizCreatorTimeEdit");
	_videoSongStartTimeEdit->setDisplayFormat("mm:ss");
	videoSettingsLayout->addWidget(_videoSongStartTimeEdit, 3, 1, 1, 1);

	/** Video Buttons - Play */
	btn = new QPushButton;
	btn->setProperty("type", "videoSong");
	btn->setObjectName("quizCreatorPlayBtn");
	connect(btn, SIGNAL(released()), this, SLOT(playSong()));
	videoSettingsLayout->addWidget(btn, 3, 2, 1, 1);

	/** Video Buttons - Pause */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorPauseBtn");
	connect(btn, SIGNAL(released()), this, SLOT(pause()));
	videoSettingsLayout->addWidget(btn, 3, 3, 1, 1);

	/** Video Buttons - Stop */
	btn = new QPushButton;
	btn->setObjectName("quizCreatorStopBtn");
	connect(btn, SIGNAL(released()), this, SLOT(stop()));
	videoSettingsLayout->addWidget(btn, 3, 4, 1, 1);

	/** Add layout to settings widget */
	_videoSettings = new QWidget;
	_videoSettings->setEnabled(false);
	_videoSettings->setLayout(videoSettingsLayout);
	mainlayout->addWidget(_videoSettings, ++row, 0, 1, 2);

	/** Set Layout */
	return mainlayout;
}

void MusicQuiz::EntryCreator::playSong()
{
	/** Sanity Check */
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr || _audioPlayer == nullptr ) {
		return;
	}

	/** Stop Media */
	stop();

	/** Get Type */
	size_t startTime = 0;
	QString fileName = "";
	const QString type = button->property("type").toString();
	if ( type == "song" ) {
		/** Sanity Check */
		if ( _songStartTimeEdit == nullptr || _songFileLineEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		fileName = _songFileLineEdit->text();

		/** Get Start and End Time */
		startTime = toMSec(_songStartTimeEdit->time());
	} else if ( type == "songAnswer" ) {
		/** Sanity Check */
		if ( _answerStartTimeEdit == nullptr || _songFileLineEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		fileName = _songFileLineEdit->text();

		/** Get Start and End Time */
		startTime = toMSec(_answerStartTimeEdit->time());
	} else if ( type == "videoSong" ) {
		/** Sanity Check */
		if ( _videoSongStartTimeEdit == nullptr || _videoSongFileLineEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		fileName = _videoSongFileLineEdit->text();

		/** Get Start and End Time */
		startTime = toMSec(_videoSongStartTimeEdit->time());
	}

	/** Check if file is valid */
	if ( fileName.isEmpty() || !isSongFileValid(fileName) ) {
		return;
	}

	/** Play Song */
	_audioPlayer->play(fileName, startTime);
}

void MusicQuiz::EntryCreator::pause()
{
	/** Pause Audio */
	if ( _audioPlayer != nullptr ) {
		_audioPlayer->pause();
	}

	/** Pause Video */
	if ( _videoPlayer != nullptr ) {
		_videoPlayer->pause();
	}
}

void MusicQuiz::EntryCreator::stop()
{
	/** Stop Audio */
	if ( _audioPlayer != nullptr ) {
		_audioPlayer->stop();
	}

	/** Stop Video */
	if ( _videoPlayer != nullptr ) {
		_videoPlayer->stop();
	}
}

void MusicQuiz::EntryCreator::playVideo()
{
	/** Sanity Check */
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr || _videoPlayer == nullptr || _audioPlayer == nullptr ) {
		return;
	}

	/** Stop Media */
	stop();

	/** Get Type */
	const QString type = button->property("type").toString();
	if ( type == "video" ) {
		/** Sanity Check */
		if ( _videoFileLineEdit == nullptr || _videoStartTimeEdit == nullptr || _videoSongFileLineEdit == nullptr || _videoSongStartTimeEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		const QString songFileName = _videoSongFileLineEdit->text();
		const QString videoFileName = _videoFileLineEdit->text();

		/** Check if file is valid */
		if ( videoFileName.isEmpty() || !isVideoFileValid(videoFileName) ) {
			return;
		}

		if ( songFileName.isEmpty() || !isSongFileValid(songFileName) ) {
			return;
		}

		/** Get Start and End Time */
		const size_t videoStartTime = toMSec(_videoStartTimeEdit->time());
		const size_t songStartTime = toMSec(_videoSongStartTimeEdit->time());

		/** Play Video and Song */
		_videoPlayer->play(videoFileName, videoStartTime, true);
		_videoPlayer->show();
		_audioPlayer->play(songFileName, songStartTime);
	} else if ( type == "videoAnswer" ) {
		/** Sanity Check */
		if ( _songFileLineEdit == nullptr || _videoAnswerStartTimeEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		const QString videoFileName = _videoFileLineEdit->text();
		if ( videoFileName.isEmpty() || !isVideoFileValid(videoFileName) ) {
			return;
		}

		/** Get Start and End Time */
		const size_t videoStartTime = toMSec(_videoAnswerStartTimeEdit->time());

		/** Play Video */
		_videoPlayer->play(videoFileName, videoStartTime);
		_videoPlayer->show();
	}
}

void MusicQuiz::EntryCreator::browseSong()
{
	/** Sanity Check */
	if ( _songFileLineEdit == nullptr ) {
		return;
	}

	/** Get Allowed Audio Formats */
	QString allowedAudioFormats = "";
	for ( size_t i = 0; i < _validAudioFormats.size(); ++i ) {
		allowedAudioFormats += "*" + _validAudioFormats[i] + "; ";
	}

	/** Open File Dialog */
	const QString filePath = QFileDialog::getOpenFileName(this, "Select Audio File", _config.getQuizDataPath().c_str(), "Audio File (" + allowedAudioFormats + ");");
	if ( filePath.isEmpty() ) {
		return;
	}

	/** Update Line Edit */
	_songFileLineEdit->setText(filePath);
}

void MusicQuiz::EntryCreator::browseVideo()
{
	/** Sanity Check */
	if ( _videoFileLineEdit == nullptr ) {
		return;
	}

	/** Get Allowed Video Formats */
	QString allowedVideoFormats = "";
	for ( size_t i = 0; i < _validVideoFormats.size(); ++i ) {
		allowedVideoFormats += "*" + _validVideoFormats[i] + "; ";
	}

	/** Open File Dialog */
	const QString filePath = QFileDialog::getOpenFileName(this, "Select Video File", _config.getQuizDataPath().c_str(), "Video File (" + allowedVideoFormats + ");");
	if ( filePath.isEmpty() ) {
		return;
	}

	/** Update Line Edit */
	_videoFileLineEdit->setText(filePath);
}

void MusicQuiz::EntryCreator::browseVideoSong()
{
	/** Sanity Check */
	if ( _videoSongFileLineEdit == nullptr ) {
		return;
	}

	/** Get Allowed Audio Formats */
	QString allowedAudioFormats = "";
	for ( size_t i = 0; i < _validAudioFormats.size(); ++i ) {
		allowedAudioFormats += "*" + _validAudioFormats[i] + "; ";
	}

	/** Open File Dialog */
	const QString filePath = QFileDialog::getOpenFileName(this, "Select Audio File", _config.getQuizDataPath().c_str(), "Audio File (" + allowedAudioFormats + ");");
	if ( filePath.isEmpty() ) {
		return;
	}

	/** Update Line Edit */
	_videoSongFileLineEdit->setText(filePath);
}

void MusicQuiz::EntryCreator::checkSongFileName()
{
	/** Sanity Check */
	if ( _songSettings == nullptr || _songFileLineEdit == nullptr ) {
		return;
	}

	/** Check if name is valid */
	bool isValid = isSongFileValid(_songFileLineEdit->text());

	QColor textColor;
	if ( isValid ) {
		/** Line Edit Color */
		textColor = QColor(0, 0, 0);

		/** Enable Song Controls */
		if ( !_songSettings->isEnabled() && _entryType == EntryType::Song ) {
			_songSettings->setEnabled(true);
		}
	} else {
		/** Line Edit Color */
		textColor = QColor(255, 0, 0);

		/** Disable Song Controls */
		if ( _songSettings->isEnabled() ) {
			_songSettings->setEnabled(false);
		}
	}

	/** Set Line Edit Color */
	if ( !_songFileLineEdit->isEnabled() ) {
		_songFileLineEdit->setStyleSheet("color: rgb(150, 150, 150);");
	} else {
		_songFileLineEdit->setStyleSheet("color: rgb(" + QString::number(textColor.red()) + "," + QString::number(textColor.green()) + "," + QString::number(textColor.blue()) + ");");
	}
}

void MusicQuiz::EntryCreator::checkVideoFiles()
{
	/** Sanity Check */
	if ( _videoSettings == nullptr || _videoFileLineEdit == nullptr || _videoSongFileLineEdit == nullptr ) {
		return;
	}

	/** Check if name is valid */
	const bool isVideoValid = isVideoFileValid(_videoFileLineEdit->text());
	const bool isSongValid = isSongFileValid(_videoSongFileLineEdit->text());

	/** Video File Line Edit */
	if ( _videoFileLineEdit->isEnabled() ) {
		if ( isVideoValid ) {
			/** Set Line Edit Color */
			_videoFileLineEdit->setStyleSheet("color: black;");
		} else {
			/** Set Line Edit Color */
			_videoFileLineEdit->setStyleSheet("color: red;");
		}
	} else {
		_videoFileLineEdit->setStyleSheet("color: rgb(150, 150, 150);");
	}

	/** Video Song File Line Edit */
	if ( _videoSongFileLineEdit->isEnabled() ) {
		if ( isSongValid ) {
			/** Set Line Edit Color */
			_videoSongFileLineEdit->setStyleSheet("color: black;");
		} else {
			/** Set Line Edit Color */
			_videoSongFileLineEdit->setStyleSheet("color: red;");
		}
	} else {
		_videoSongFileLineEdit->setStyleSheet("color: rgb(150, 150, 150);");
	}

	/** Video Controls */
	if ( isVideoValid && isSongValid ) {
		/** Enable Video Controls */
		if ( !_videoSettings->isEnabled() && _entryType == EntryType::Video ) {
			_videoSettings->setEnabled(true);
		}
	} else {
		/** Disable Video Controls */
		if ( _videoSettings->isEnabled() ) {
			_videoSettings->setEnabled(false);
		}
	}
}

bool MusicQuiz::EntryCreator::isSongFileValid(const QString& fileName)
{
	/** Check if file has a valid format */
	bool validFormat = false;
	for ( size_t i = 0; i < _validAudioFormats.size(); ++i ) {
		if ( fileName.toLower().contains(_validAudioFormats[i]) ) {
			validFormat = true;
			break;
		}
	}

	if ( !validFormat ) {
		return false;
	}

	/** Check if file exists */
	if ( !filesystem::exists(fileName.toStdString()) ) {
		return false;
	}

	return true;
}

bool MusicQuiz::EntryCreator::isVideoFileValid(const QString& fileName)
{
	/** Check if file has a valid format */
	bool validFormat = false;
	for ( size_t i = 0; i < _validVideoFormats.size(); ++i ) {
		if ( fileName.toLower().contains(_validVideoFormats[i]) ) {
			validFormat = true;
			break;
		}
	}

	if ( !validFormat ) {
		return false;
	}

	/** Check if file exists */
	if ( !filesystem::exists(fileName.toStdString()) ) {
		return false;
	}

	return true;
}

size_t MusicQuiz::EntryCreator::toMSec(const QTime& time)
{
	return time.minute() * 60000 + time.second() * 1000;
}

QTime MusicQuiz::EntryCreator::fromMSec(size_t time)
{
	/** Hour */
	const size_t minute = time / 60000;
	time = time - 60000 * minute;

	/** Minute */
	const size_t second = time / 1000;
	time = time - 1000 * second;

	/** Return */
	return QTime(0, static_cast<int>(minute), static_cast<int>(second), static_cast<int>(time));
}

void MusicQuiz::EntryCreator::setEntryType(int index)
{
	/** Sanity Check */
	if ( _videoSettings == nullptr || _browseVideoBtn == nullptr || _videoFileLineEdit == nullptr ) {
		return;
	}

	if ( index == 0 ) { // song
		/** Set Type */
		_entryType = EntryType::Song;

		/** Set Video Minimum Size */
		_videoPlayer->setMinimumSize(QSize(0, 0));
		_videoPlayer->resize(QSize(0, 0));

		/** Enable Song Settings */
		_songSettings->setEnabled(true);
		_browseSongBtn->setEnabled(true);
		_songFileLineEdit->setEnabled(true);

		/** Disable Video Widgets */
		_videoSettings->setEnabled(false);
		_browseVideoBtn->setEnabled(false);
		_browseVideoSongBtn->setEnabled(false);
		_videoFileLineEdit->setEnabled(false);
		_videoSongFileLineEdit->setEnabled(false);
	} else if ( index == 1 ) { // video
		/** Set Type */
		_entryType = EntryType::Video;

		/** Set Video Minimum Size */
		int width = 0;
		int height = 0;
		if ( parentWidget()->parentWidget() != nullptr ) {
			width = this->parentWidget()->parentWidget()->width();
			height = int(this->parentWidget()->parentWidget()->width() * 0.75);
		} else {
			width = this->width();
			height = int(this->width() * 0.75);
		}
		_videoPlayer->setMinimumSize(QSize(width / 2, height / 2));
		_videoPlayer->resize(QSize(width / 2, height / 2));

		/** Disable Song Settings */
		_songSettings->setEnabled(false);
		_browseSongBtn->setEnabled(false);
		_songFileLineEdit->setEnabled(false);

		/** Enable Video Widgets */
		_videoSettings->setEnabled(true);
		_browseVideoBtn->setEnabled(true);
		_browseVideoSongBtn->setEnabled(true);
		_videoFileLineEdit->setEnabled(true);
		_videoSongFileLineEdit->setEnabled(true);
	}

	checkVideoFiles();
	checkSongFileName();
}

void MusicQuiz::EntryCreator::setName(const QString& name)
{
	/** Sanity Check */
	if ( _entryNameLabel == nullptr ) {
		return;
	}

	/** Set Name */
	_entryName = name;

	/** Update Label */
	_entryNameLabel->setText(_entryName);
}

const QString MusicQuiz::EntryCreator::getName() const
{
	return _entryName;
}

void MusicQuiz::EntryCreator::pointsChanged(int points)
{
	_points = points;
}

void MusicQuiz::EntryCreator::setPoints(const int points)
{
	/** Sanity Check */
	if ( _pointsSpinbox == nullptr ) {
		return;
	}

	/** Set Answer */
	_pointsSpinbox->setValue(points);
}

size_t MusicQuiz::EntryCreator::getPoints() const
{
	return _points;
}

void MusicQuiz::EntryCreator::setType(const EntryType& type)
{
	/** Sanity Check */
	if ( _pointsSpinbox == nullptr ) {
		return;
	}

	/** Set Type */
	_entryType = type;
	if ( _entryType == EntryType::Song ) {
		_buttonGroup->button(0)->setChecked(true);
		setEntryType(0);
	} else if ( _entryType == EntryType::Video ) {
		_buttonGroup->button(1)->setChecked(true);
		setEntryType(1);
	}
}

MusicQuiz::EntryCreator::EntryType MusicQuiz::EntryCreator::getType() const
{
	return _entryType;
}

void MusicQuiz::EntryCreator::setSongFile(const QString& file)
{
	/** Sanity Check */
	if ( _songFileLineEdit == nullptr ) {
		return;
	}

	/** Set File */
	_songFileLineEdit->setText(file);
}

const QString MusicQuiz::EntryCreator::getSongFile()
{
	/** Sanity Check */
	if ( _songFileLineEdit == nullptr ) {
		return "";
	}

	/** Check if file is valid */
	if ( isSongFileValid(_songFileLineEdit->text()) ) {
		return _songFileLineEdit->text();
	}

	return "";
}

void MusicQuiz::EntryCreator::setVideoFile(const QString& file)
{
	/** Sanity Check */
	if ( _videoFileLineEdit == nullptr ) {
		return;
	}

	/** Set File */
	_videoFileLineEdit->setText(file);
}

const QString MusicQuiz::EntryCreator::getVideoFile()
{
	/** Sanity Check */
	if ( _videoFileLineEdit == nullptr ) {
		return "";
	}

	/** Check if file is valid */
	if ( isVideoFileValid(_videoFileLineEdit->text()) ) {
		return _videoFileLineEdit->text();
	}

	return "";
}

void MusicQuiz::EntryCreator::setVideoSongFile(const QString& file)
{
	/** Sanity Check */
	if ( _videoSongFileLineEdit == nullptr ) {
		return;
	}

	/** Set File */
	_videoSongFileLineEdit->setText(file);
}

const QString MusicQuiz::EntryCreator::getVideoSongFile()
{
	/** Sanity Check */
	if ( _videoSongFileLineEdit == nullptr ) {
		return "";
	}

	/** Check if file is valid */
	if ( isSongFileValid(_videoSongFileLineEdit->text()) ) {
		return _videoSongFileLineEdit->text();
	}

	return "";
}

void MusicQuiz::EntryCreator::setSongStartTime(const size_t time)
{
	/** Sanity Check */
	if ( _songStartTimeEdit == nullptr ) {
		return;
	}

	/** Set Time */
	_songStartTimeEdit->setTime(fromMSec(time));
}

size_t MusicQuiz::EntryCreator::getSongStartTime()
{
	/** Sanity Check */
	if ( _songStartTimeEdit == nullptr ) {
		return 0;
	}

	return toMSec(_songStartTimeEdit->time());
}

void MusicQuiz::EntryCreator::setAnswerStartTime(const size_t time)
{
	/** Sanity Check */
	if ( _answerStartTimeEdit == nullptr ) {
		return;
	}

	/** Set Time */
	_answerStartTimeEdit->setTime(fromMSec(time));
}

size_t MusicQuiz::EntryCreator::getAnswerStartTime()
{
	/** Sanity Check */
	if ( _answerStartTimeEdit == nullptr ) {
		return 0;
	}

	return toMSec(_answerStartTimeEdit->time());
}

void MusicQuiz::EntryCreator::setVideoStartTime(const size_t time)
{
	/** Sanity Check */
	if ( _videoStartTimeEdit == nullptr ) {
		return;
	}

	/** Set Time */
	_videoStartTimeEdit->setTime(fromMSec(time));
}

size_t MusicQuiz::EntryCreator::getVideoStartTime()
{
	/** Sanity Check */
	if ( _videoStartTimeEdit == nullptr ) {
		return 0;
	}

	return toMSec(_videoStartTimeEdit->time());
}

void MusicQuiz::EntryCreator::setVideoSongStartTime(const size_t time)
{
	/** Sanity Check */
	if ( _videoSongStartTimeEdit == nullptr ) {
		return;
	}

	/** Set Time */
	_videoSongStartTimeEdit->setTime(fromMSec(time));
}

size_t MusicQuiz::EntryCreator::getVideoSongStartTime()
{
	/** Sanity Check */
	if ( _videoSongStartTimeEdit == nullptr ) {
		return 0;
	}

	return toMSec(_videoSongStartTimeEdit->time());
}

void MusicQuiz::EntryCreator::setVideoAnswerStartTime(const size_t time)
{
	/** Sanity Check */
	if ( _videoAnswerStartTimeEdit == nullptr ) {
		return;
	}

	/** Set Time */
	_videoAnswerStartTimeEdit->setTime(fromMSec(time));
}

size_t MusicQuiz::EntryCreator::getVideoAnswerStartTime()
{
	/** Sanity Check */
	if ( _videoAnswerStartTimeEdit == nullptr ) {
		return 0;
	}

	return toMSec(_videoAnswerStartTimeEdit->time());
}