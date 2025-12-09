#include "EntryCreator.hpp"

#include <math.h>
#include <functional>
#include <filesystem>

#include <QTime>
#include <QVoice>
#include <QLabel>
#include <QString>
#include <QVector>
#include <QPixmap>
#include <QScrollArea>
#include <QFileDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QMediaContent>
#include <QTextToSpeech>

#include "common/Configuration.hpp"

#include "gui_tools/GuiUtil/QExtensions/QSliderWidget.hpp"


MusicQuiz::EntryCreator::EntryCreator(const QString& name, const int points, const media::AudioPlayer::Ptr& audioPlayer,
	const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer, const common::Configuration& config, QWidget* parent) :
	QWidget(parent), _points(points), _entryName(name), _audioPlayer(audioPlayer), _textToSpeechPlayer(textToSpeechPlayer), _config(config)
{
	/** Create Layout */
	createLayout();
}

MusicQuiz::EntryCreator::EntryCreator(const boost::property_tree::ptree &tree, const media::AudioPlayer::Ptr& audioPlayer,
	const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer, const common::Configuration& config, QWidget* parent) :
	QWidget(parent), _points(tree.get<int>("Points")), _entryName(QString::fromStdString(tree.get<std::string>("<xmlattr>.name"))),
	_audioPlayer(audioPlayer),	_textToSpeechPlayer(textToSpeechPlayer), _config(config)
{
	createLayout();
	const std::string type = tree.get<std::string>("<xmlattr>.type");
	if ( type == "song" ) {
		loadSongFromXml(tree);
	} else if ( type == "video" ) {
		loadVideoFromXml(tree);
	} else if ( type == "textToSpeech" ) {
		loadTextToSpeechFromXml(tree);
	} else if( type == "image" ) {
		loadImageFromXml(tree);
	} else if ( type == "text" ) {
		loadTextFromXml(tree);
	}
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
	_pointsSpinbox->setSingleStep(50);
	_pointsSpinbox->setValue(_points);
	connect(_pointsSpinbox, SIGNAL(valueChanged(int)), this, SLOT(pointsChanged(int)));
	mainlayout->addWidget(_pointsSpinbox, row, 1);

	/** Type */
	QHBoxLayout* typeLayoutTop = new QHBoxLayout;
	QHBoxLayout* typeLayoutBottom = new QHBoxLayout;
	typeLayoutTop->setStretch(0, 1);
	typeLayoutTop->setStretch(1, 1);
	typeLayoutTop->setStretch(2, 1);
	typeLayoutTop->setStretch(3, 1);
	typeLayoutBottom->setStretch(0, 1);
	typeLayoutBottom->setStretch(1, 1);
	typeLayoutBottom->setStretch(2, 1);
	typeLayoutBottom->setStretch(3, 1);
	label = new QLabel("Type:");
	label->setObjectName("quizCreatorLabel");
	typeLayoutTop->addWidget(label);
	typeLayoutBottom->addWidget(new QWidget());

	_buttonGroup = new QButtonGroup;
	QRadioButton* songBtn = new QRadioButton("Song");
	QRadioButton* videoBtn = new QRadioButton("Video");
	QRadioButton* textToSpeechBtn = new QRadioButton("Text to Speech");
	QRadioButton* imageBtn = new QRadioButton("Image");
	QRadioButton* textBtn = new QRadioButton("Text");
	songBtn->setObjectName("quizCreatorRadioButton");
	videoBtn->setObjectName("quizCreatorRadioButton");
	textToSpeechBtn->setObjectName("quizCreatorRadioButton");
	imageBtn->setObjectName("quizCreatorRadioButton");
	textBtn->setObjectName("quizCreatorRadioButton");
	_buttonGroup->addButton(songBtn, 0);
	_buttonGroup->addButton(videoBtn, 1);
	_buttonGroup->addButton(textToSpeechBtn, 2);
	_buttonGroup->addButton(imageBtn, 3);
	_buttonGroup->addButton(textBtn, 4);
	connect(_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setEntryType(int)));
	songBtn->setChecked(true);

	typeLayoutTop->addWidget(songBtn);
	typeLayoutTop->addWidget(videoBtn);
	typeLayoutTop->addWidget(textToSpeechBtn);
	typeLayoutBottom->addWidget(imageBtn);
	typeLayoutBottom->addWidget(textBtn);
	typeLayoutBottom->addWidget(new QWidget());
	mainlayout->addItem(typeLayoutTop, ++row, 0, 1, 2);
	mainlayout->addItem(typeLayoutBottom, ++row, 0, 1, 2);

	/** Song Layout */
	_songLayout = new QWidget;
	_songLayout->setLayout(createSongLayout());
	mainlayout->addWidget(_songLayout, ++row, 0, 1, 2);

	/** Video Layout */
	_videoLayout = new QWidget;
	_videoLayout->setLayout(createVideoLayout());
	mainlayout->addWidget(_videoLayout, ++row, 0, 1, 2);

	/** Text to Speech Layout */
	_textToSpeechLayout = new QWidget;
	_textToSpeechLayout->setLayout(createTextToSpeechLayout());
	mainlayout->addWidget(_textToSpeechLayout, ++row, 0, 1, 2);

	/** Image Layout */
	_imageLayout = new QWidget;
	_imageLayout->setLayout(createImageLayout());
	mainlayout->addWidget(_imageLayout, ++row, 0, 1, 2);

	/** Text Layout */
	_textLayout = new QWidget;
	_textLayout->setLayout(createTextLayout());
	mainlayout->addWidget(_textLayout, ++row, 0, 1, 2);

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

QGridLayout* MusicQuiz::EntryCreator::createSongLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QHBoxLayout* songFileLayout = new QHBoxLayout;
	QGridLayout* songSettingsLayout = new QGridLayout;
	songSettingsLayout->setHorizontalSpacing(5);
	songSettingsLayout->setVerticalSpacing(10);
	songSettingsLayout->setMargin(0);
	songFileLayout->setMargin(0);
	mainlayout->setVerticalSpacing(10);
	mainlayout->setMargin(0);
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

QGridLayout* MusicQuiz::EntryCreator::createVideoLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QHBoxLayout* videoLayout = new QHBoxLayout;
	QHBoxLayout* videoFileLayout = new QHBoxLayout;
	QHBoxLayout* videoSongFileLayout = new QHBoxLayout;
	QGridLayout* videoSettingsLayout = new QGridLayout;
	videoSettingsLayout->setHorizontalSpacing(5);
	videoSettingsLayout->setVerticalSpacing(10);
	videoSettingsLayout->setMargin(0);
	videoSongFileLayout->setMargin(0);
	videoFileLayout->setMargin(0);
	mainlayout->setVerticalSpacing(10);
	mainlayout->setMargin(0);
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
	label = new QLabel("Song File:");
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

QGridLayout* MusicQuiz::EntryCreator::createTextToSpeechLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QHBoxLayout* textToSpeechLayout = new QHBoxLayout;
	QGridLayout* textToSpeechSettingsLayout = new QGridLayout;
	QHBoxLayout* textToSpeechAnswerSongFileLayout = new QHBoxLayout;
	QGridLayout* textToSpeechAnswerSettingsLayout = new QGridLayout;
	textToSpeechAnswerSettingsLayout->setMargin(0);
	textToSpeechSettingsLayout->setMargin(0);
	mainlayout->setVerticalSpacing(10);
	mainlayout->setMargin(0);
	int row = 0;

	/** Song - Lyrics */
	QLabel* label = new QLabel("Song Lyrics:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	textToSpeechLayout->setSpacing(10);
	textToSpeechLayout->setMargin(0);
	_textToSpeechTextEdit = new QTextEdit;
	_textToSpeechTextEdit->setAcceptRichText(false);
	_textToSpeechTextEdit->setObjectName("quizCreatorTextToSpeechTextEdit");
	_textToSpeechTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(_textToSpeechTextEdit, SIGNAL(textChanged()), this, SLOT(checkTextToSpeechLyrics()));
	textToSpeechLayout->addWidget(_textToSpeechTextEdit);
	mainlayout->addItem(textToSpeechLayout, ++row, 0, 1, 2);

	/** Answer Song - File */
	label = new QLabel("Answer Song File:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	textToSpeechAnswerSongFileLayout->setSpacing(10);
	_textToSpeechAnswerSongFileLineEdit = new QLineEdit;
	_textToSpeechAnswerSongFileLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_textToSpeechAnswerSongFileLineEdit->setObjectName("quizCreatorLineEdit");
	connect(_textToSpeechAnswerSongFileLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkTextToSpeechAnswerSongFileName()));
	textToSpeechAnswerSongFileLayout->addWidget(_textToSpeechAnswerSongFileLineEdit);

	_browseTextToSpeechAnswerSongBtn = new QPushButton;
	_browseTextToSpeechAnswerSongBtn->setObjectName("quizCreatorBrowseBtn");
	connect(_browseTextToSpeechAnswerSongBtn, SIGNAL(released()), this, SLOT(browseTextToSpeechAnswerSong()));
	textToSpeechAnswerSongFileLayout->addWidget(_browseTextToSpeechAnswerSongBtn);
	mainlayout->addItem(textToSpeechAnswerSongFileLayout, ++row, 0, 1, 2);

	/** Start */
	label = new QLabel("Start:");
	label->setObjectName("quizCreatorLabel");
	textToSpeechSettingsLayout->addWidget(label, 0, 0, 1, 1);

	/** Text to Speech Buttons - Play */
	QPushButton* playBtn = new QPushButton;
	playBtn->setProperty("type", "textToSpeech");
	playBtn->setObjectName("quizCreatorPlayBtn");
	connect(playBtn, SIGNAL(released()), this, SLOT(playText()));
	textToSpeechSettingsLayout->addWidget(playBtn, 0, 2, 1, 1);

	/** Text to Speech Buttons - Pause */
	QPushButton* pauseBtn = new QPushButton;
	pauseBtn->setObjectName("quizCreatorPauseBtn");
	connect(pauseBtn, SIGNAL(released()), this, SLOT(pause()));
	textToSpeechSettingsLayout->addWidget(pauseBtn, 0, 3, 1, 1);

	/** Text to Speech Buttons - Stop */
	QPushButton* stopBtn = new QPushButton;
	stopBtn->setObjectName("quizCreatorStopBtn");
	connect(stopBtn, SIGNAL(released()), this, SLOT(stop()));
	textToSpeechSettingsLayout->addWidget(stopBtn, 0, 4, 1, 1);

	/** Add layout to settings widget */
	_textToSpeechSettings = new QWidget;
	_textToSpeechSettings->setEnabled(false);
	_textToSpeechSettings->setLayout(textToSpeechSettingsLayout);
	mainlayout->addWidget(_textToSpeechSettings, ++row, 0, 1, 2);

	/** Song - Set Answer Start */
	label = new QLabel("Answer:");
	label->setObjectName("quizCreatorLabel");
	textToSpeechAnswerSettingsLayout->addWidget(label, 1, 0, 1, 1);

	/** Answer Start Time */
	_textToSpeechAnswerStartTimeEdit = new QTimeEdit;
	_textToSpeechAnswerStartTimeEdit->setAlignment(Qt::AlignCenter);
	_textToSpeechAnswerStartTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_textToSpeechAnswerStartTimeEdit->setObjectName("quizCreatorTimeEdit");
	_textToSpeechAnswerStartTimeEdit->setDisplayFormat("mm:ss");
	textToSpeechAnswerSettingsLayout->addWidget(_textToSpeechAnswerStartTimeEdit, 1, 1, 1, 1);

	/** Answer Audio Buttons - Play */
	QPushButton* answerPlayBtn = new QPushButton;
	answerPlayBtn->setProperty("type", "textToSpeechSongAnswer");
	answerPlayBtn->setObjectName("quizCreatorPlayBtn");
	connect(answerPlayBtn, SIGNAL(released()), this, SLOT(playSong()));
	textToSpeechAnswerSettingsLayout->addWidget(answerPlayBtn, 1, 2, 1, 1);

	/** Answer Audio Buttons - Pause */
	QPushButton* answerPauseBtn = new QPushButton;
	answerPauseBtn->setObjectName("quizCreatorPauseBtn");
	connect(answerPauseBtn, SIGNAL(released()), this, SLOT(pause()));
	textToSpeechAnswerSettingsLayout->addWidget(answerPauseBtn, 1, 3, 1, 1);

	/** Answer Audio Buttons - Stop */
	QPushButton* answerStopBtn = new QPushButton;
	answerStopBtn->setObjectName("quizCreatorStopBtn");
	connect(answerStopBtn, SIGNAL(released()), this, SLOT(stop()));
	textToSpeechAnswerSettingsLayout->addWidget(answerStopBtn, 1, 4, 1, 1);

	/** Add layout to answer settings widget */
	_textToSpeechAnswerSettings = new QWidget;
	_textToSpeechAnswerSettings->setEnabled(false);
	_textToSpeechAnswerSettings->setLayout(textToSpeechAnswerSettingsLayout);
	mainlayout->addWidget(_textToSpeechAnswerSettings, ++row, 0, 1, 2);

	/** Pitch */
	_pitchSlider = new gui_tools::GuiUtil::QSliderWidget("Pitch", -1.0, 1.0, 21, "", "", 1);
	_pitchSlider->setValue(0.0);
	mainlayout->addWidget(_pitchSlider, ++row, 0, 1, 2);

	/** Rate */
	_rateSlider = new gui_tools::GuiUtil::QSliderWidget("Playback Rate", -1.0, 1.0, 21, "", "", 1);
	_rateSlider->setValue(0.0);
	mainlayout->addWidget(_rateSlider, ++row, 0, 1, 2);

	/** Voices - Label */
	label = new QLabel("Voices:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0, 1, 2);

	/** Voices - Set avaliable voices */
	QGridLayout* voicesLayout = new QGridLayout;
	_voiceButtonGroup = new QButtonGroup;
	QVector< QVoice > availableVoices = _textToSpeechPlayer->availableVoices();
	for ( int i = 0; i < availableVoices.size(); ++i ) {
		/** Get Name */
		const QString voiceName = availableVoices[i].name() + " (" + QVoice::genderName(availableVoices[i].gender()) + ", " + QVoice::ageName(availableVoices[i].age()) + ")";

		/** Create Button */
		QRadioButton* createButton = new QRadioButton(voiceName);
		createButton->setObjectName("quizCreatorRadioButtonVoices");
		createButton->setProperty("voiceName", availableVoices[i].name());
		if ( i == 0 ) {
			createButton->setChecked(true);
		}
		_voiceButtonGroup->addButton(createButton, i);

		/** Add button to layout */
		voicesLayout->addWidget(createButton, static_cast<int>(std::floor(i / 2)), i % 2);
	}

	mainlayout->addItem(voicesLayout, ++row, 0, 1, 2);

	/** Set Layout */
	return mainlayout;
}

QGridLayout* MusicQuiz::EntryCreator::createImageLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QHBoxLayout* imageLayout = new QHBoxLayout;
	QHBoxLayout* imageFileLayout = new QHBoxLayout;
	QGridLayout* imageSettingsLayout = new QGridLayout;
	QHBoxLayout* imageAnswerSongFileLayout = new QHBoxLayout;
	QGridLayout* imageAnswerSettingsLayout = new QGridLayout;
	imageAnswerSettingsLayout->setMargin(0);
	imageSettingsLayout->setMargin(0);
	mainlayout->setVerticalSpacing(10);
	mainlayout->setMargin(0);
	int row = 0;

	/** Image - File */
	QLabel* label = new QLabel("Image File:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	imageFileLayout->setSpacing(10);
	_imageFileLineEdit = new QLineEdit;
	_imageFileLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_imageFileLineEdit->setObjectName("quizCreatorLineEdit");
	connect(_imageFileLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkImageFiles()));
	imageFileLayout->addWidget(_imageFileLineEdit);

	_browseImageBtn = new QPushButton;
	_browseImageBtn->setObjectName("quizCreatorBrowseBtn");
	connect(_browseImageBtn, SIGNAL(released()), this, SLOT(browseImage()));
	imageFileLayout->addWidget(_browseImageBtn);
	mainlayout->addItem(imageFileLayout, ++row, 0, 1, 2);

	/** Image */
	imageLayout->setSpacing(10);
	imageLayout->setMargin(0);
	_imagePreviewLabel = new QLabel;
	_imagePreviewLabel->setObjectName("quizCreatorImagePreviewLabel");
	_imagePreviewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_imagePreviewLabel->setAlignment(Qt::AlignCenter);
	imageLayout->addWidget(_imagePreviewLabel, Qt::AlignCenter);
	mainlayout->addItem(imageLayout, ++row, 0, 1, 2);

	/** Answer Song - File */
	label = new QLabel("Answer Song File:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	imageAnswerSongFileLayout->setSpacing(10);
	_imageAnswerSongFileLineEdit = new QLineEdit;
	_imageAnswerSongFileLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_imageAnswerSongFileLineEdit->setObjectName("quizCreatorLineEdit");
	connect(_imageAnswerSongFileLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkImageFiles()));
	imageAnswerSongFileLayout->addWidget(_imageAnswerSongFileLineEdit);

	_browseImageAnswerSongBtn = new QPushButton;
	_browseImageAnswerSongBtn->setObjectName("quizCreatorBrowseBtn");
	connect(_browseImageAnswerSongBtn, SIGNAL(released()), this, SLOT(browseImageAnswerSong()));
	imageAnswerSongFileLayout->addWidget(_browseImageAnswerSongBtn);
	mainlayout->addItem(imageAnswerSongFileLayout, ++row, 0, 1, 2);

	/** Add layout to settings widget */
	_textToSpeechSettings = new QWidget;
	_textToSpeechSettings->setEnabled(false);
	_textToSpeechSettings->setLayout(imageAnswerSettingsLayout);
	mainlayout->addWidget(_textToSpeechSettings, ++row, 0, 1, 2);

	/** Song - Set Answer Start */
	label = new QLabel("Answer:");
	label->setObjectName("quizCreatorLabel");
	imageAnswerSettingsLayout->addWidget(label, 1, 0, 1, 1);

	/** Answer Start Time */
	_imageAnswerStartTimeEdit = new QTimeEdit;
	_imageAnswerStartTimeEdit->setAlignment(Qt::AlignCenter);
	_imageAnswerStartTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_imageAnswerStartTimeEdit->setObjectName("quizCreatorTimeEdit");
	_imageAnswerStartTimeEdit->setDisplayFormat("mm:ss");
	imageAnswerSettingsLayout->addWidget(_imageAnswerStartTimeEdit, 1, 1, 1, 1);

	/** Answer Audio Buttons - Play */
	QPushButton* answerPlayBtn = new QPushButton;
	answerPlayBtn->setProperty("type", "imageSongAnswer");
	answerPlayBtn->setObjectName("quizCreatorPlayBtn");
	connect(answerPlayBtn, SIGNAL(released()), this, SLOT(playSong()));
	imageAnswerSettingsLayout->addWidget(answerPlayBtn, 1, 2, 1, 1);

	/** Answer Audio Buttons - Pause */
	QPushButton* answerPauseBtn = new QPushButton;
	answerPauseBtn->setObjectName("quizCreatorPauseBtn");
	connect(answerPauseBtn, SIGNAL(released()), this, SLOT(pause()));
	imageAnswerSettingsLayout->addWidget(answerPauseBtn, 1, 3, 1, 1);

	/** Answer Audio Buttons - Stop */
	QPushButton* answerStopBtn = new QPushButton;
	answerStopBtn->setObjectName("quizCreatorStopBtn");
	connect(answerStopBtn, SIGNAL(released()), this, SLOT(stop()));
	imageAnswerSettingsLayout->addWidget(answerStopBtn, 1, 4, 1, 1);

	/** Add layout to answer settings widget */
	_imageAnswerSettings = new QWidget;
	_imageAnswerSettings->setEnabled(false);
	_imageAnswerSettings->setLayout(imageAnswerSettingsLayout);
	mainlayout->addWidget(_imageAnswerSettings, ++row, 0, 1, 2);

	/** Set Layout */
	return mainlayout;
}

QGridLayout* MusicQuiz::EntryCreator::createTextLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QHBoxLayout* textLayout = new QHBoxLayout;
	QGridLayout* textSettingsLayout = new QGridLayout;
	QHBoxLayout* textAnswerSongFileLayout = new QHBoxLayout;
	QGridLayout* textAnswerSettingsLayout = new QGridLayout;
	textAnswerSettingsLayout->setMargin(0);
	textSettingsLayout->setMargin(0);
	mainlayout->setVerticalSpacing(10);
	mainlayout->setMargin(0);
	int row = 0;

	/** Text */
	QLabel* label = new QLabel("Text:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	textLayout->setSpacing(10);
	textLayout->setMargin(0);
	_textTextEdit = new QTextEdit;
	_textTextEdit->setAcceptRichText(false);
	_textTextEdit->setObjectName("quizCreatorTextTextEdit");
	_textTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLayout->addWidget(_textTextEdit);
	mainlayout->addItem(textLayout, ++row, 0, 1, 2);

	/** Answer Song - File */
	label = new QLabel("Answer Song File:");
	label->setObjectName("quizCreatorLabel");
	mainlayout->addWidget(label, ++row, 0);

	textAnswerSongFileLayout->setSpacing(10);
	_textAnswerSongFileLineEdit = new QLineEdit;
	_textAnswerSongFileLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_textAnswerSongFileLineEdit->setObjectName("quizCreatorLineEdit");
	connect(_textAnswerSongFileLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkTextAnswerSongFileName()));
	textAnswerSongFileLayout->addWidget(_textAnswerSongFileLineEdit);

	_browseTextAnswerSongBtn = new QPushButton;
	_browseTextAnswerSongBtn->setObjectName("quizCreatorBrowseBtn");
	connect(_browseTextAnswerSongBtn, SIGNAL(released()), this, SLOT(browseTextAnswerSong()));
	textAnswerSongFileLayout->addWidget(_browseTextAnswerSongBtn);
	mainlayout->addItem(textAnswerSongFileLayout, ++row, 0, 1, 2);

	/** Song - Set Answer Start */
	label = new QLabel("Answer:");
	label->setObjectName("quizCreatorLabel");
	textAnswerSettingsLayout->addWidget(label, 1, 0, 1, 1);

	/** Answer Start Time */
	_textAnswerStartTimeEdit = new QTimeEdit;
	_textAnswerStartTimeEdit->setAlignment(Qt::AlignCenter);
	_textAnswerStartTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	_textAnswerStartTimeEdit->setObjectName("quizCreatorTimeEdit");
	_textAnswerStartTimeEdit->setDisplayFormat("mm:ss");
	textAnswerSettingsLayout->addWidget(_textAnswerStartTimeEdit, 1, 1, 1, 1);

	/** Answer Audio Buttons - Play */
	QPushButton* answerPlayBtn = new QPushButton;
	answerPlayBtn->setProperty("type", "textSongAnswer");
	answerPlayBtn->setObjectName("quizCreatorPlayBtn");
	connect(answerPlayBtn, SIGNAL(released()), this, SLOT(playSong()));
	textAnswerSettingsLayout->addWidget(answerPlayBtn, 1, 2, 1, 1);

	/** Answer Audio Buttons - Pause */
	QPushButton* answerPauseBtn = new QPushButton;
	answerPauseBtn->setObjectName("quizCreatorPauseBtn");
	connect(answerPauseBtn, SIGNAL(released()), this, SLOT(pause()));
	textAnswerSettingsLayout->addWidget(answerPauseBtn, 1, 3, 1, 1);

	/** Answer Audio Buttons - Stop */
	QPushButton* answerStopBtn = new QPushButton;
	answerStopBtn->setObjectName("quizCreatorStopBtn");
	connect(answerStopBtn, SIGNAL(released()), this, SLOT(stop()));
	textAnswerSettingsLayout->addWidget(answerStopBtn, 1, 4, 1, 1);

	/** Add layout to answer settings widget */
	_textAnswerSettings = new QWidget;
	_textAnswerSettings->setEnabled(false);
	_textAnswerSettings->setLayout(textAnswerSettingsLayout);
	mainlayout->addWidget(_textAnswerSettings, ++row, 0, 1, 2);

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
	std::filesystem::path fileName = "";
	const QString type = button->property("type").toString();
	if ( type == "song" ) {
		/** Sanity Check */
		if ( _songStartTimeEdit == nullptr || _songFileLineEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		fileName = _songFileLineEdit->text().toStdString();

		/** Get Start and End Time */
		startTime = toMSec(_songStartTimeEdit->time());
	} else if ( type == "songAnswer" ) {
		/** Sanity Check */
		if ( _answerStartTimeEdit == nullptr || _songFileLineEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		fileName = _songFileLineEdit->text().toStdString();

		/** Get Start and End Time */
		startTime = toMSec(_answerStartTimeEdit->time());
	} else if ( type == "videoSong" ) {
		/** Sanity Check */
		if ( _videoSongStartTimeEdit == nullptr || _videoSongFileLineEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		fileName = _videoSongFileLineEdit->text().toStdString();

		/** Get Start and End Time */
		startTime = toMSec(_videoSongStartTimeEdit->time());
	} else if ( type == "textToSpeechSongAnswer" ) {
		/** Sanity Check */
		if (_textToSpeechAnswerStartTimeEdit == nullptr || _textToSpeechAnswerSongFileLineEdit == nullptr) {
			return;
		}

		/** Get File Name */
		fileName = _textToSpeechAnswerSongFileLineEdit->text().toStdString();

		/** Get Start and End Time */
		startTime = toMSec( _textToSpeechAnswerStartTimeEdit->time() );
	} else if( type == "imageSongAnswer" ) {
		/** Sanity Check */
		if( _imageAnswerStartTimeEdit == nullptr || _imageAnswerSongFileLineEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		fileName = _imageAnswerSongFileLineEdit->text().toStdString();

		/** Get Start and End Time */
		startTime = toMSec( _imageAnswerStartTimeEdit->time() );
	} else if ( type == "textSongAnswer" ) {
		/** Sanity Check */
		if ( _textAnswerStartTimeEdit == nullptr || _textAnswerSongFileLineEdit == nullptr ) {
			return;
		}

		/** Get File Name */
		fileName = _textAnswerSongFileLineEdit->text().toStdString();

		/** Get Start and End Time */
		startTime = toMSec(_textAnswerStartTimeEdit->time());
	}

	/** Check if file is valid */
	if ( fileName.empty() || !isSongFileValid(QString::fromStdString(fileName.string())) ) {
		return;
	}

	/** Play Song */
	_audioPlayer->play(fileName, startTime);
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
		const std::filesystem::path songFileName = _videoSongFileLineEdit->text().toStdString();
		const std::filesystem::path videoFileName = _videoFileLineEdit->text().toStdString();

		/** Check if file is valid */
		if ( videoFileName.empty() || !isVideoFileValid(QString::fromStdString(videoFileName.string())) ) {
			return;
		}

		if ( songFileName.empty() || !isSongFileValid(QString::fromStdString(songFileName.string())) ) {
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
		const std::filesystem::path videoFileName = _videoFileLineEdit->text().toStdString();
		if ( videoFileName.empty() || !isVideoFileValid(QString::fromStdString(videoFileName.string())) ) {
			return;
		}

		/** Get Start and End Time */
		const size_t videoStartTime = toMSec(_videoAnswerStartTimeEdit->time());

		/** Play Video */
		_videoPlayer->play(videoFileName, videoStartTime);
		_videoPlayer->show();
	}
}

void MusicQuiz::EntryCreator::playText()
{
	/** Sanity Check */
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr || _textToSpeechPlayer == nullptr || _textToSpeechTextEdit == nullptr || _pitchSlider == nullptr || _rateSlider == nullptr ) {
		return;
	}

	/** Stop Media */
	stop();

	/** Get String */
	QString string = _textToSpeechTextEdit->toPlainText();
	if ( string.isEmpty() ) {
		return;
	}

	/** Get Answer String */
	const bool playAnswer = (button->property("type").toString() == "textToSpeechAnswer");

	/** Get Settings */
	media::TextToSpeechPlayer::TextToSpeechSettings settings;
	settings._pitch = _pitchSlider->getValue();
	settings._rate = _rateSlider->getValue();

	/** Get Voice */
	QAbstractButton* selectedButton = _voiceButtonGroup->checkedButton();
	if ( selectedButton != nullptr ) {
		const QString selectedVoice = selectedButton->property("voiceName").toString();
		const QVector< QVoice > availableVoices = _textToSpeechPlayer->availableVoices();
		for ( int i = 0; i < availableVoices.size(); ++i ) {
			if ( selectedVoice == availableVoices[i].name() ) {
				settings._voice = availableVoices[i];
				break;
			}
		}	
	}

	/** Play Song */
	_textToSpeechPlayer->play(string, settings, playAnswer);
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

	/** Pause Text to Speech */
	if ( _textToSpeechPlayer != nullptr ) {
		_textToSpeechPlayer->pause();
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

	/** Stop Text to Speech */
	if ( _textToSpeechPlayer != nullptr ) {
		_textToSpeechPlayer->stop();
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
		allowedAudioFormats += "*" + _validAudioFormats[i] + " ";
	}

	/** Open File Dialog */
	const QString filePath = QFileDialog::getOpenFileName(this, "Select Audio File", _config.getQuizDataPath().c_str(), "Audio File (" + allowedAudioFormats + ")");
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

void MusicQuiz::EntryCreator::browseTextToSpeechAnswerSong()
{
	/** Sanity Check */
	if ( _textToSpeechAnswerSongFileLineEdit == nullptr ) {
		return;
	}

	/** Get Allowed Audio Formats */
	QString allowedAudioFormats = "";
	for (size_t i = 0; i < _validAudioFormats.size(); ++i) {
		allowedAudioFormats += "*" + _validAudioFormats[i] + "; ";
	}

	/** Open File Dialog */
	const QString filePath = QFileDialog::getOpenFileName(this, "Select Audio File", _config.getQuizDataPath().c_str(), "Audio File (" + allowedAudioFormats + ");");
	if ( filePath.isEmpty() ) {
		return;
	}

	/** Update Line Edit */
	_textToSpeechAnswerSongFileLineEdit->setText(filePath);
}

void MusicQuiz::EntryCreator::browseImage()
{
	/** Sanity Check */
	if ( _imageFileLineEdit == nullptr ) {
		return;
	}

	/** Get Allowed Image Formats */
	QString allowedImageFormats = "";
	for (size_t i = 0; i < _validImageFormats.size(); ++i) {
		allowedImageFormats += "*" + _validImageFormats[i] + " ";
	}

	/** Open File Dialog */
	const QString filePath = QFileDialog::getOpenFileName(this, "Select Image File", _config.getQuizDataPath().c_str(), "Image File (" + allowedImageFormats + ")");
	if (filePath.isEmpty()) {
		return;
	}

	/** Update Line Edit */
	_imageFileLineEdit->setText(filePath);

	/** Set Image */
	QPixmap pixmap(filePath);
	_imagePreviewLabel->setPixmap(pixmap.scaled(_imagePreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MusicQuiz::EntryCreator::browseImageAnswerSong()
{
	/** Sanity Check */
	if( _imageAnswerSongFileLineEdit == nullptr ) {
		return;
	}

	/** Get Allowed Audio Formats */
	QString allowedAudioFormats = "";
	for( size_t i = 0; i < _validAudioFormats.size(); ++i ) {
		allowedAudioFormats += "*" + _validAudioFormats[i] + "; ";
	}

	/** Open File Dialog */
	const QString filePath = QFileDialog::getOpenFileName(this, "Select Audio File", _config.getQuizDataPath().c_str(), "Audio File (" + allowedAudioFormats + ");");
	if( filePath.isEmpty() ) {
		return;
	}

	/** Update Line Edit */
	_imageAnswerSongFileLineEdit->setText(filePath);
}

void MusicQuiz::EntryCreator::browseTextAnswerSong()
{
	/** Sanity Check */
	if ( _textAnswerSongFileLineEdit == nullptr ) {
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
	_textAnswerSongFileLineEdit->setText(filePath);
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

void MusicQuiz::EntryCreator::checkTextToSpeechLyrics()
{
	/** Sanity Check */
	if ( _textToSpeechTextEdit == nullptr ) {
		return;
	}

	/** Get string */
	const QString string = _textToSpeechTextEdit->toPlainText();

	/** Check if lyrics are valid */
	bool textValid = true;
	bool answerValid = true;

	if ( string.isEmpty() ) {
		textValid = false;
		answerValid = false;
	}

	/** Get text color */
	QColor textColor = QColor(0, 0, 0);
	if ( !textValid || !answerValid ) {
		textColor = QColor(255, 0, 0);
	}

	/** Set Text Edit Color */
	_textToSpeechTextEdit->setStyleSheet("color: rgb(" + QString::number(textColor.red()) + "," + QString::number(textColor.green()) + "," + QString::number(textColor.blue()) + ");");

	/** Set Widget enabled / disabled */
	if ( textValid != _textToSpeechSettings->isEnabled() ) {
		_textToSpeechSettings->setEnabled(textValid);
	}
}

void MusicQuiz::EntryCreator::checkTextToSpeechAnswerSongFileName()
{
	/** Sanity Check */
	if (_textToSpeechAnswerSongFileLineEdit == nullptr) {
		return;
	}

	/** Check if name is valid */
	bool isValid = isSongFileValid(_textToSpeechAnswerSongFileLineEdit->text());

	QColor textColor;
	if (isValid) {
		/** Line Edit Color */
		textColor = QColor(0, 0, 0);
	} else {
		/** Line Edit Color */
		textColor = QColor(255, 0, 0);
	}

	/** Set Widget enabled / disabled */
	if (isValid != _textToSpeechAnswerSettings->isEnabled()) {
		_textToSpeechAnswerSettings->setEnabled(isValid);
	}

	/** Set Line Edit Color */
	if (!_textToSpeechAnswerSongFileLineEdit->isEnabled()) {
		_textToSpeechAnswerSongFileLineEdit->setStyleSheet("color: rgb(150, 150, 150);");
	} else {
		_textToSpeechAnswerSongFileLineEdit->setStyleSheet("color: rgb(" + QString::number(textColor.red()) + "," + QString::number(textColor.green()) + "," + QString::number(textColor.blue()) + ");");
	}
}

void MusicQuiz::EntryCreator::checkImageFiles()
{
	/** Sanity Check */
	if ( _imageFileLineEdit == nullptr || _imageAnswerSongFileLineEdit == nullptr ) {
		return;
	}

	/** Check if name is valid */
	bool isImageValid = isImageFileValid(_imageFileLineEdit->text());
	bool isAnswerSongValid = isSongFileValid(_imageAnswerSongFileLineEdit->text());

	/** Image File Line Edit */
	if ( _imageFileLineEdit->isEnabled() ) {
		if ( isImageValid ) {
			/** Set Line Edit Color */
			_imageFileLineEdit->setStyleSheet("color: black;");
		}
		else {
			/** Set Line Edit Color */
			_imageFileLineEdit->setStyleSheet("color: red;");
		}
	} else {
		_imageFileLineEdit->setStyleSheet("color: rgb(150, 150, 150);");
	}

	/** Image Answer Song File Line Edit */
	if ( _imageAnswerSongFileLineEdit->isEnabled()) {
		if ( isAnswerSongValid ) {
			/** Set Line Edit Color */
			_imageAnswerSongFileLineEdit->setStyleSheet("color: black;");
		} else {
			/** Set Line Edit Color */
			_imageAnswerSongFileLineEdit->setStyleSheet("color: red;");
		}
	} else {
		_imageAnswerSongFileLineEdit->setStyleSheet("color: rgb(150, 150, 150);");
	}

	/** Set Widget enabled / disabled */
	if( isAnswerSongValid != _imageAnswerSettings->isEnabled() ) {
		_imageAnswerSettings->setEnabled(isAnswerSongValid);
	}
}

void MusicQuiz::EntryCreator::checkTextAnswerSongFileName()
{
	/** Sanity Check */
	if ( _textAnswerSongFileLineEdit == nullptr ) {
		return;
	}

	/** Check if name is valid */
	bool isValid = isSongFileValid(_textAnswerSongFileLineEdit->text());

	QColor textColor;
	if ( isValid ) {
		/** Line Edit Color */
		textColor = QColor(0, 0, 0);
	}
	else {
		/** Line Edit Color */
		textColor = QColor(255, 0, 0);
	}

	/** Set Widget enabled / disabled */
	if ( isValid != _textAnswerSettings->isEnabled() ) {
		_textAnswerSettings->setEnabled(isValid);
	}

	/** Set Line Edit Color */
	if ( !_textAnswerSongFileLineEdit->isEnabled() ) {
		_textAnswerSongFileLineEdit->setStyleSheet("color: rgb(150, 150, 150);");
	}
	else {
		_textAnswerSongFileLineEdit->setStyleSheet("color: rgb(" + QString::number(textColor.red()) + "," + QString::number(textColor.green()) + "," + QString::number(textColor.blue()) + ");");
	}
}

bool MusicQuiz::EntryCreator::isSongFileValid(const QString& fileName) const
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
	if ( !std::filesystem::exists(fileName.toStdString()) ) {
		return false;
	}

	return true;
}

bool MusicQuiz::EntryCreator::isVideoFileValid(const QString& fileName) const
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
	if ( !std::filesystem::exists(fileName.toStdString()) ) {
		return false;
	}

	return true;
}

bool MusicQuiz::EntryCreator::isImageFileValid( const QString& fileName ) const
{
	/** Check if file has a valid format */
	bool validFormat = false;
	for( size_t i = 0; i < _validImageFormats.size(); ++i ) {
		if( fileName.toLower().contains( _validImageFormats[i] ) ) {
			validFormat = true;
			break;
		}
	}

	if( !validFormat ) {
		return false;
	}

	/** Check if file exists */
	if( !std::filesystem::exists( fileName.toStdString() ) ) {
		return false;
	}

	return true;
}

size_t MusicQuiz::EntryCreator::toMSec(const QTime& time) const
{
	return time.minute() * 60000 + time.second() * 1000;
}

QTime MusicQuiz::EntryCreator::fromMSec(size_t time) const
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

		/** Show / hide layouts */
		_songLayout->show();
		_videoLayout->hide();
		_textToSpeechLayout->hide();
		_imageLayout->hide();
		_textLayout->hide();
	} else if ( index == 1 ) { // video
		/** Set Type */
		_entryType = EntryType::Video;

		/** Set Video Minimum Size */
		int width = 0;
		int height = 0;
		if ( parentWidget()->parentWidget() != nullptr ) {
			width = this->parentWidget()->parentWidget()->width();
			height = static_cast< int >(this->parentWidget()->parentWidget()->width() * 0.75);
		} else {
			width = this->width();
			height = static_cast< int >(this->width() * 0.75);
		}
		_videoPlayer->setMinimumSize(QSize(width / 2, height / 2));
		_videoPlayer->resize(QSize(width / 2, height / 2));

		/** Show / hide layouts */
		_videoLayout->show();
		_songLayout->hide();
		_textToSpeechLayout->hide();
		_imageLayout->hide();
		_textLayout->hide();
	} else if ( index == 2 ) { // text to speech
		/** Set Type */
		_entryType = EntryType::TextToSpeech;

		/** Show / hide layouts */
		_textToSpeechLayout->show();
		_songLayout->hide();
		_videoLayout->hide();
		_imageLayout->hide();
		_textLayout->hide();
	} else if (index == 3) { // image
		/** Set Type */
		_entryType = EntryType::Image;

		/** Show / hide layouts */
		_imageLayout->show();
		_songLayout->hide();
		_videoLayout->hide();
		_textToSpeechLayout->hide();
		_textLayout->hide();
	} else if ( index == 4 ) { // text
		/** Set Type */
		_entryType = EntryType::Text;

		/** Show / hide layouts */
		_textLayout->show();
		_songLayout->hide();
		_videoLayout->hide();
		_textToSpeechLayout->hide();
		_imageLayout->hide();
	}

	checkVideoFiles();
	checkSongFileName();
	checkTextToSpeechLyrics();
	checkTextToSpeechAnswerSongFileName();
	checkImageFiles();
	checkTextAnswerSongFileName();
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
	} else if ( _entryType == EntryType::TextToSpeech ) {
		_buttonGroup->button(2)->setChecked(true);
		setEntryType(2);
	} else if (_entryType == EntryType::Image) {
		_buttonGroup->button(3)->setChecked(true);
		setEntryType(3);
	} else if ( _entryType == EntryType::Text ) {
		_buttonGroup->button(4)->setChecked(true);
		setEntryType(4);
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

const QString MusicQuiz::EntryCreator::getSongFile() const
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

const QString MusicQuiz::EntryCreator::getVideoFile() const
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

const QString MusicQuiz::EntryCreator::getVideoSongFile() const
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

void MusicQuiz::EntryCreator::setTextToSpeechAnswerSongFile(const QString& file)
{
	/** Sanity Check */
	if (_textToSpeechAnswerSongFileLineEdit == nullptr) {
		return;
	}

	/** Set File */
	_textToSpeechAnswerSongFileLineEdit->setText(file);
}

const QString MusicQuiz::EntryCreator::getTextToSpeechAnswerSongFile() const
{
	/** Sanity Check */
	if (_textToSpeechAnswerSongFileLineEdit == nullptr) {
		return "";
	}

	/** Check if file is valid */
	if (isSongFileValid(_textToSpeechAnswerSongFileLineEdit->text())) {
		return _textToSpeechAnswerSongFileLineEdit->text();
	}

	return "";
}

void MusicQuiz::EntryCreator::setImageFile(const QString& file)
{
	/** Sanity Check */
	if ( _imageFileLineEdit == nullptr ) {
		return;
	}

	/** Set File */
	_imageFileLineEdit->setText(file);

	/** Set Image */
	QPixmap pixmap(file);
	_imagePreviewLabel->setPixmap(pixmap.scaled(_imagePreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

const QString MusicQuiz::EntryCreator::getImageFile() const
{
	/** Sanity Check */
	if ( _imageFileLineEdit == nullptr ) {
		return "";
	}

	/** Check if file is valid */
	if ( isImageFileValid(_imageFileLineEdit->text()) ) {
		return _imageFileLineEdit->text();
	}

	return "";
}

void MusicQuiz::EntryCreator::setImageAnswerSongFile(const QString& file)
{
	/** Sanity Check */
	if( _imageAnswerSongFileLineEdit == nullptr ) {
		return;
	}

	/** Set File */
	_imageAnswerSongFileLineEdit->setText(file);
}

const QString MusicQuiz::EntryCreator::getImageAnswerSongFile() const
{
	/** Sanity Check */
	if( _imageAnswerSongFileLineEdit == nullptr ) {
		return "";
	}

	/** Check if file is valid */
	if( isSongFileValid(_imageAnswerSongFileLineEdit->text()) ) {
		return _imageAnswerSongFileLineEdit->text();
	}

	return "";
}

void MusicQuiz::EntryCreator::setTextAnswerSongFile(const QString& file)
{
	/** Sanity Check */
	if ( _textAnswerSongFileLineEdit == nullptr ) {
		return;
	}

	/** Set File */
	_textAnswerSongFileLineEdit->setText(file);
}

const QString MusicQuiz::EntryCreator::getTextAnswerSongFile() const
{
	/** Sanity Check */
	if ( _textAnswerSongFileLineEdit == nullptr ) {
		return "";
	}

	/** Check if file is valid */
	if ( isSongFileValid(_textAnswerSongFileLineEdit->text()) ) {
		return _textAnswerSongFileLineEdit->text();
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

size_t MusicQuiz::EntryCreator::getSongStartTime() const
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

size_t MusicQuiz::EntryCreator::getAnswerStartTime() const
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

size_t MusicQuiz::EntryCreator::getVideoStartTime() const
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

size_t MusicQuiz::EntryCreator::getVideoSongStartTime() const
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

size_t MusicQuiz::EntryCreator::getVideoAnswerStartTime() const
{
	/** Sanity Check */
	if ( _videoAnswerStartTimeEdit == nullptr ) {
		return 0;
	}

	return toMSec(_videoAnswerStartTimeEdit->time());
}

void MusicQuiz::EntryCreator::setTextToSpeechString(const QString& textToSpeechString)
{
	/** Sanity Check */
	if ( _textToSpeechTextEdit == nullptr ) {
		return;
	}

	/** Set Text to Speech String */
	_textToSpeechTextEdit->setAcceptRichText(true);
	_textToSpeechTextEdit->setText(textToSpeechString);
	_textToSpeechTextEdit->setAcceptRichText(false);
}

QString MusicQuiz::EntryCreator::getTextToSpeechString() const
{
	/** Sanity Check */
	if ( _textToSpeechTextEdit == nullptr ) {
		return "";
	}

	return _textToSpeechTextEdit->toPlainText();
}

void MusicQuiz::EntryCreator::setTextToSpeechAnswerStartTime(const size_t time)
{
	/** Sanity Check */
	if (_textToSpeechAnswerStartTimeEdit == nullptr) {
		return;
	}

	/** Set Time */
	_textToSpeechAnswerStartTimeEdit->setTime(fromMSec(time));
}

size_t MusicQuiz::EntryCreator::getTextToSpeechAnswerStartTime() const
{
	/** Sanity Check */
	if (_textToSpeechAnswerStartTimeEdit == nullptr) {
		return 0;
	}

	return toMSec(_textToSpeechAnswerStartTimeEdit->time());
}

void MusicQuiz::EntryCreator::setPitch(double pitch)
{
	/** Sanity Check */
	if ( _pitchSlider == nullptr ) {
		return;
	}

	if ( pitch < -1.0 || pitch > 1.0 ) {
		return;
	}

	_pitchSlider->setValue(pitch);
}

double MusicQuiz::EntryCreator::getPitch() const
{
	/** Sanity Check */
	if ( _pitchSlider == nullptr ) {
		return 0.0;
	}

	return _pitchSlider->getValue();
}

void MusicQuiz::EntryCreator::setRate(double rate)
{
	/** Sanity Check */
	if ( _rateSlider == nullptr ) {
		return;
	}

	if ( rate < -1.0 || rate > 1.0 ) {
		return;
	}

	_rateSlider->setValue(rate);
}

double MusicQuiz::EntryCreator::getRate() const
{
	/** Sanity Check */
	if ( _rateSlider == nullptr ) {
		return 0.0;
	}

	return _rateSlider->getValue();
}

void MusicQuiz::EntryCreator::setVoice(const QString& voiceName)
{
	/** Sanity Check */
	if ( _voiceButtonGroup == nullptr || _textToSpeechPlayer == nullptr ) {
		return;
	}

	/** Check voice exists */
	bool voiceExists = false;
	const QVector< QVoice > availableVoices = _textToSpeechPlayer->availableVoices();
	for ( int i = 0; i < availableVoices.size(); ++i ) {
		if ( voiceName == availableVoices[i].name() ) {
			voiceExists = true;
			break;
		}
	}

	if ( !voiceExists ) {
		return;
	}

	/** Find button with correct name */
	for ( int i = 0; i < _voiceButtonGroup->buttons().size(); ++i ) {
		QAbstractButton* btn = _voiceButtonGroup->button(i);
		if ( btn != nullptr ) {
			if ( btn->property("voiceName").toString() == voiceName ) {
				btn->setChecked(true);
				return;
			}
		}
	}
}

QString MusicQuiz::EntryCreator::getVoiceName() const
{
	/** Sanity Check */
	if ( _voiceButtonGroup == nullptr ) {
		return "";
	}

	/** Get voice name */
	QAbstractButton* selectedButton = _voiceButtonGroup->checkedButton();
	if ( selectedButton != nullptr ) {
		return selectedButton->property("voiceName").toString();
	}
	
	return "";
}

void MusicQuiz::EntryCreator::setImageAnswerStartTime(const size_t time)
{
	/** Sanity Check */
	if( _imageAnswerStartTimeEdit == nullptr ) {
		return;
	}

	/** Set Time */
	_imageAnswerStartTimeEdit->setTime(fromMSec(time));
}

size_t MusicQuiz::EntryCreator::getImageAnswerStartTime() const
{
	/** Sanity Check */
	if( _imageAnswerStartTimeEdit == nullptr ) {
		return 0;
	}

	return toMSec(_imageAnswerStartTimeEdit->time());
}


void MusicQuiz::EntryCreator::setTextString(const QString& textToSpeechString)
{
	/** Sanity Check */
	if ( _textTextEdit == nullptr ) {
		return;
	}

	/** Set Text to Speech String */
	_textTextEdit->setAcceptRichText(true);
	_textTextEdit->setText(textToSpeechString);
	_textTextEdit->setAcceptRichText(false);
}

QString MusicQuiz::EntryCreator::getTextString() const
{
	/** Sanity Check */
	if ( _textTextEdit == nullptr ) {
		return "";
	}

	return _textTextEdit->toPlainText();
}

void MusicQuiz::EntryCreator::setTextAnswerStartTime(const size_t time)
{
	/** Sanity Check */
	if ( _textAnswerStartTimeEdit == nullptr ) {
		return;
	}

	/** Set Time */
	_textAnswerStartTimeEdit->setTime(fromMSec(time));
}

size_t MusicQuiz::EntryCreator::getTextAnswerStartTime() const
{
	/** Sanity Check */
	if ( _textAnswerStartTimeEdit == nullptr ) {
		return 0;
	}

	return toMSec(_textAnswerStartTimeEdit->time());
}

void MusicQuiz::EntryCreator::loadSongFromXml(const boost::property_tree::ptree &tree)
{
	/** Set Type */
	setType(MusicQuiz::EntryCreator::EntryType::Song);

	/** Set Song Start Time */
	try {
		setSongStartTime(tree.get<size_t>("StartTime"));
	} catch ( ... ) {}

	/** Set Answer Start Time */
	try {
		setAnswerStartTime(tree.get<size_t>("AnswerStartTime"));
	} catch ( ... ) {}

	/** Set Song File */
	try {
		QString songFile = QString::fromStdString(_config.mediaPathToFullPath(tree.get<std::string>("Media.SongFile")));
		std::replace(songFile.begin(), songFile.end(), '\\', '/');
		setSongFile(songFile);
	} catch ( ... ) {}
}

void MusicQuiz::EntryCreator::loadVideoFromXml(const boost::property_tree::ptree &tree)
{
	/** Set Type */
	setType(MusicQuiz::EntryCreator::EntryType::Video);

	/** Set Song Start Time */
	try {
		setVideoSongStartTime(tree.get<size_t>("VideoSongStartTime"));
	} catch ( ... ) {}

	/** Set Video Start Time */
	try {
		setVideoStartTime(tree.get<size_t>("StartTime"));
	} catch ( ... ) {}

	/** Set Video Answer Start Time */
	try {
		setVideoAnswerStartTime(tree.get<size_t>("AnswerStartTime"));
	} catch ( ... ) {}

	/** Set Video File */
	try {
		QString videoFile = QString::fromStdString(_config.mediaPathToFullPath(tree.get<std::string>("Media.VideoFile")));
		std::replace(videoFile.begin(), videoFile.end(), '\\', '/');
		setVideoFile(videoFile);
	} catch ( ... ) {}

	/** Set Song File */
	try {
		QString songFile = QString::fromStdString(_config.mediaPathToFullPath(tree.get<std::string>("Media.SongFile")));
		std::replace(songFile.begin(), songFile.end(), '\\', '/');
		setVideoSongFile(songFile);
	} catch ( ... ) {}
}

void MusicQuiz::EntryCreator::loadTextToSpeechFromXml(const boost::property_tree::ptree& tree)
{
	/** Set Type */
	setType(MusicQuiz::EntryCreator::EntryType::TextToSpeech);

	/** Set Text to Speech String */
	try {
		setTextToSpeechString(QString::fromStdString(tree.get<std::string>("Media.TextToSpeechString")));
	} catch ( ... ) {}

	/** Set Answer Song File */
	try {
		QString songFile = QString::fromStdString(_config.mediaPathToFullPath(tree.get<std::string>("Media.SongFile")));
		std::replace(songFile.begin(), songFile.end(), '\\', '/');
		setTextToSpeechAnswerSongFile(songFile);
	}
	catch (...) {}

	/** Set Answer Song Start Time */
	try {
		setTextToSpeechAnswerStartTime(tree.get<size_t>("AnswerStartTime"));
	}
	catch (...) {}

	/** Set Pitch */
	try {
		setPitch(tree.get<double>("Media.Pitch", 0.0));
	} catch ( ... ) {}

	/** Set Rate */
	try {
		setRate(tree.get<double>("Media.Rate", 0.0));
	} catch ( ... ) {}

	/** Set Voice */
	try {
		setVoice(QString::fromStdString(tree.get<std::string>("Media.VoiceName", "")));
	} catch ( ... ) {}
}

void MusicQuiz::EntryCreator::loadImageFromXml(const boost::property_tree::ptree& tree)
{
	/** Set Type */
	setType(MusicQuiz::EntryCreator::EntryType::Image);

	/** Set Text to Speech String */
	try {
		setImageFile(QString::fromStdString(_config.mediaPathToFullPath(tree.get<std::string>("Media.ImageFile"))));
	} catch ( ... ) {}

	/** Set Answer Song File */
	try {
		QString songFile = QString::fromStdString(_config.mediaPathToFullPath(tree.get<std::string>("Media.SongFile")));
		std::replace(songFile.begin(), songFile.end(), '\\', '/');
		setImageAnswerSongFile(songFile);
	}
	catch (...) {}

	/** Set Answer Song Start Time */
	try {
		setImageAnswerStartTime(tree.get<size_t>("AnswerStartTime"));
	}
	catch (...) {}
}

void MusicQuiz::EntryCreator::loadTextFromXml(const boost::property_tree::ptree& tree)
{
	/** Set Type */
	setType(MusicQuiz::EntryCreator::EntryType::Text);

	/** Set Text String */
	try {
		setTextString(QString::fromStdString(tree.get<std::string>("Media.TextString")));
	}
	catch ( ... ) {}

	/** Set Answer Song File */
	try {
		QString songFile = QString::fromStdString(_config.mediaPathToFullPath(tree.get<std::string>("Media.SongFile")));
		std::replace(songFile.begin(), songFile.end(), '\\', '/');
		setTextAnswerSongFile(songFile);
	}
	catch ( ... ) {}

	/** Set Answer Song Start Time */
	try {
		setTextAnswerStartTime(tree.get<size_t>("AnswerStartTime"));
	}
	catch ( ... ) {}
}

boost::property_tree::ptree MusicQuiz::EntryCreator::toXml(const std::string& savePath, const std::string xmlPath) const
{
	std::string name = getName().toStdString();
	boost::property_tree::ptree tree;
	tree.put("Answer", name);
	tree.put("<xmlattr>.name", name);
	tree.put("Points", getPoints());
	switch(getType()) {
		case MusicQuiz::EntryCreator::EntryType::Song:
			saveSongToXml(tree, savePath + "/" + name, xmlPath + "/" + name);
			break;
		case MusicQuiz::EntryCreator::EntryType::Video:
			saveVideoToXml(tree, savePath + "/" + name, xmlPath + "/" + name);
			break;
		case MusicQuiz::EntryCreator::EntryType::TextToSpeech:
			saveTextToSpeechToXml(tree, savePath + "/" + name, xmlPath + "/" + name);
			break;
		case MusicQuiz::EntryCreator::EntryType::Image:
			saveImageToXml(tree, savePath + "/" + name, xmlPath + "/" + name);
			break;
		case MusicQuiz::EntryCreator::EntryType::Text:
			saveTextToXml(tree, savePath + "/" + name, xmlPath + "/" + name);
			break;
		default:
			break;
	}
	return tree;
}

void MusicQuiz::EntryCreator::saveSongToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const
{
	/** Entry Type */
	tree.put("<xmlattr>.type", "song");

	/** Entry Song Start Time */
	tree.put("StartTime", getSongStartTime());

	/** Entry Song Answer Start Time */
	tree.put("AnswerStartTime", getAnswerStartTime());

	/** Media File */
	const std::string songFile = getSongFile().toStdString();
	if ( !songFile.empty() ) {
		const std::string audioFileExtension = std::filesystem::path(songFile).extension().string();
		boost::property_tree::ptree& media_tree = tree.add("Media", "");
		media_tree.put("SongFile", std::filesystem::relative(xmlPath + audioFileExtension, _config.getQuizDataPath()).string());

		/** Copy Media File */
		std::filesystem::copy_file(songFile, savePath + audioFileExtension, std::filesystem::copy_options::overwrite_existing);
	}
}

void MusicQuiz::EntryCreator::saveVideoToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const
{
	/** Entry Type */
	tree.put("<xmlattr>.type", "video");

	/** Entry Video Start Time */
	tree.put("StartTime", getVideoStartTime());

	/** Entry Video Song Start Time */
	tree.put("VideoSongStartTime", getVideoSongStartTime());

	/** Entry Video Answer Start Time */
	tree.put("AnswerStartTime", getVideoAnswerStartTime());

	/** Media File */
	const std::string videoFile = getVideoFile().toStdString();
	const std::string songFile = getVideoSongFile().toStdString();

	if ( !videoFile.empty() && !songFile.empty() ) {
		const std::string videoFileExtension = std::filesystem::path(videoFile).extension().string();
		const std::string audioFileExtension = std::filesystem::path(songFile).extension().string();
		boost::property_tree::ptree& media_tree = tree.add("Media", "");
		media_tree.put("VideoFile", std::filesystem::relative(xmlPath + "_video" + videoFileExtension, _config.getQuizDataPath()).string());
		media_tree.put("SongFile", std::filesystem::relative(xmlPath + "_song" + audioFileExtension, _config.getQuizDataPath()).string());

		/** Copy Media File */
		std::filesystem::copy_file(videoFile, savePath + "_video" + videoFileExtension, std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file(songFile, savePath + "_song" + audioFileExtension, std::filesystem::copy_options::overwrite_existing);
	}
}

void MusicQuiz::EntryCreator::saveTextToSpeechToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const
{
	/** Entry Type */
	tree.put("<xmlattr>.type", "textToSpeech");

	/** Entry Answer Song Start Time */
	tree.put("AnswerStartTime", getTextToSpeechAnswerStartTime());

	/** Media File */
	const std::string songFile = getTextToSpeechAnswerSongFile().toStdString();
	const std::string textToSpeechString = getTextToSpeechString().toStdString();
	if ( !textToSpeechString.empty() && !songFile.empty() ) {
		boost::property_tree::ptree& media_tree = tree.add("Media", "");
		media_tree.put("TextToSpeechString", textToSpeechString);
		media_tree.put("Pitch", getPitch());
		media_tree.put("Rate", getRate());
		media_tree.put("VoiceName", getVoiceName().toStdString());

		const std::string audioFileExtension = std::filesystem::path(songFile).extension().string();
		media_tree.put("SongFile", std::filesystem::relative(xmlPath + audioFileExtension, _config.getQuizDataPath()).string());

		/** Copy Media File */
		std::filesystem::copy_file(songFile, savePath + audioFileExtension, std::filesystem::copy_options::overwrite_existing);
	}
}

void MusicQuiz::EntryCreator::saveImageToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const
{
	/** Entry Type */
	tree.put("<xmlattr>.type", "image");

	/** Entry Answer Song Start Time */
	tree.put("AnswerStartTime", getImageAnswerStartTime());

	/** Media File */
	const std::string imageFile = getImageFile().toStdString();
	const std::string songFile = getImageAnswerSongFile().toStdString();
	if ( !imageFile.empty() && !songFile.empty() ) {
		boost::property_tree::ptree& media_tree = tree.add("Media", "");

		const std::string imageFileExtension = std::filesystem::path(imageFile).extension().string();
		media_tree.put("ImageFile", std::filesystem::relative(xmlPath + imageFileExtension, _config.getQuizDataPath()).string());

		const std::string audioFileExtension = std::filesystem::path(songFile).extension().string();
		media_tree.put("SongFile", std::filesystem::relative(xmlPath + audioFileExtension, _config.getQuizDataPath()).string());

		/** Copy Media File */
		std::filesystem::copy_file(imageFile, savePath + imageFileExtension, std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file(songFile, savePath + audioFileExtension, std::filesystem::copy_options::overwrite_existing);
	}
}

void MusicQuiz::EntryCreator::saveTextToXml(boost::property_tree::ptree& tree, const std::string& savePath, const std::string& xmlPath) const
{
	/** Entry Type */
	tree.put("<xmlattr>.type", "text");

	/** Entry Answer Song Start Time */
	tree.put("AnswerStartTime", getTextAnswerStartTime());

	/** Media File */
	const std::string songFile = getTextAnswerSongFile().toStdString();
	const std::string textString = getTextString().toStdString();
	if ( !textString.empty() && !songFile.empty() ) {
		boost::property_tree::ptree& media_tree = tree.add("Media", "");
		media_tree.put("TextString", textString);

		const std::string audioFileExtension = std::filesystem::path(songFile).extension().string();
		media_tree.put("SongFile", std::filesystem::relative(xmlPath + audioFileExtension, _config.getQuizDataPath()).string());

		/** Copy Media File */
		std::filesystem::copy_file(songFile, savePath + audioFileExtension, std::filesystem::copy_options::overwrite_existing);
	}
}
