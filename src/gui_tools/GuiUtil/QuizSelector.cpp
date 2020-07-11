#include "QuizSelector.hpp"

#include <sstream>
#include <stdexcept>

#include <QLabel>
#include <QWidget>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QListWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QMessageBox>
#include <QListWidgetItem>

#include "common/Log.hpp"
#include "common/Configuration.hpp"

#include "util/QuizSettings.hpp"
#include "gui_tools/widgets/QuizSettingsDialog.hpp"


MusicQuiz::QuizSelector::QuizSelector(const common::Configuration& config, QWidget* parent) :
	QDialog(parent), _config(config)
{
	/** Set Window Flags */
	setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);

	/** Set Object Name */
	setObjectName("QuizSelector");

	/** Load Quizzes */
	_quizList = MusicQuiz::util::QuizLoader::getListOfQuizzes(config);
	if ( _quizList.empty() ) {
		throw std::runtime_error("No quizzes available.");
	}

	/** Load Quiz Previews */
	for ( size_t i = 0; i < _quizList.size(); ++i ) {
		try {
			_quizPreviews.push_back(MusicQuiz::util::QuizLoader::getQuizPreview(i, config));
		} catch ( const std::exception& err ) {
			LOG_ERROR("Failed to load quiz #" << i << ". " << err.what());
		} catch ( ... ) {
			LOG_ERROR("Failed to load quiz #" << i << ".");
		}
	}

	/** Create Layout */
	createLayout();

	/** Set Fullscreen */
	showFullScreen();
}

void MusicQuiz::QuizSelector::createLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QVBoxLayout* quizSelectionLayout = new QVBoxLayout;
	QVBoxLayout* descriptionLayout = new QVBoxLayout;
	QVBoxLayout* infoLayout = new QVBoxLayout;
	QGridLayout* categoryLayout = new QGridLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	mainlayout->setHorizontalSpacing(50);
	mainlayout->setVerticalSpacing(20);
	quizSelectionLayout->setSpacing(20);
	buttonLayout->setSpacing(20);
	infoLayout->setSpacing(30);
	descriptionLayout->setSpacing(20);
	categoryLayout->setHorizontalSpacing(20);
	categoryLayout->setVerticalSpacing(20);
	mainlayout->setColumnStretch(0, 1);
	mainlayout->setColumnStretch(1, 4);

	/** Quiz Selection List */
	_quizSelectionList = new QListWidget;
	_quizSelectionList->setSpacing(0);
	_quizSelectionList->setObjectName("quizSelectionList");
	quizSelectionLayout->addWidget(_quizSelectionList);
	connect(_quizSelectionList, SIGNAL(itemSelectionChanged()), this, SLOT(selectionClicked()));
	_quizSelectionList->setCurrentRow(0);

	/** Add Quizzes */
	for ( size_t i = 0; i < _quizPreviews.size(); ++i ) {
		QListWidgetItem* quizName = new QListWidgetItem;
		quizName->setSizeHint(QSize(100, 100));
		quizName->setTextAlignment(Qt::AlignCenter);
		quizName->setText(QString(" ") + QString::fromStdString(_quizPreviews[i].quizName));
		_quizSelectionList->addItem(quizName);
	}

	/** Description */
	QLabel* descriptionLabel = new QLabel("Description");
	descriptionLabel->setObjectName("descriptionLabel");
	descriptionLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
	descriptionLabel->setAlignment(Qt::AlignHCenter);
	descriptionLayout->addWidget(descriptionLabel);

	_descriptionText = new QTextEdit;
	_descriptionText->setReadOnly(true);
	_descriptionText->setObjectName("descriptionTextLabel");
	_descriptionText->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
	descriptionLayout->addWidget(_descriptionText);

	/** Categories */
	QLabel* categoryLabel = new QLabel("Categories");
	categoryLabel->setObjectName("categoryLabel");
	categoryLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
	categoryLabel->setAlignment(Qt::AlignLeft);
	categoryLayout->addWidget(categoryLabel, 0, 0, Qt::AlignLeft);

	_categoryText = new QTextEdit;
	_categoryText->setReadOnly(true);
	_categoryText->setObjectName("descriptionTextLabel");
	_categoryText->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
	categoryLayout->addWidget(_categoryText, 1, 0, Qt::AlignLeft);

	/** Row Categories */
	QLabel* rowCategoryLabel = new QLabel("Row Categories");
	rowCategoryLabel->setObjectName("categoryLabel");
	rowCategoryLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
	rowCategoryLabel->setAlignment(Qt::AlignLeft);
	categoryLayout->addWidget(rowCategoryLabel, 0, 1, Qt::AlignLeft);

	_rowCategoryText = new QTextEdit;
	_rowCategoryText->setReadOnly(true);
	_rowCategoryText->setObjectName("descriptionTextLabel");
	_rowCategoryText->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
	categoryLayout->addWidget(_rowCategoryText, 1, 1, Qt::AlignLeft);

	/** Quiz Info */
	QLabel* infoLabel = new QLabel("Info");
	infoLabel->setObjectName("infoLabel");
	infoLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
	infoLabel->setAlignment(Qt::AlignCenter);
	categoryLayout->addWidget(infoLabel, 0, 2, Qt::AlignLeft);

	_includeSongsCheckbox = new QCheckBox(" Includes Songs");
	_includeSongsCheckbox->setObjectName("infoCheckbox");
	_includeSongsCheckbox->setAttribute(Qt::WA_TransparentForMouseEvents);
	_includeSongsCheckbox->setFocusPolicy(Qt::NoFocus);
	infoLayout->addWidget(_includeSongsCheckbox, Qt::AlignLeft);

	_includeVideosCheckbox = new QCheckBox(" Includes Videos");
	_includeVideosCheckbox->setObjectName("infoCheckbox");
	_includeVideosCheckbox->setAttribute(Qt::WA_TransparentForMouseEvents);
	_includeVideosCheckbox->setFocusPolicy(Qt::NoFocus);
	infoLayout->addWidget(_includeVideosCheckbox, Qt::AlignLeft);

	_guessTheCategoryCheckbox = new QCheckBox(" Guess the Category");
	_guessTheCategoryCheckbox->setObjectName("infoCheckbox");
	_guessTheCategoryCheckbox->setAttribute(Qt::WA_TransparentForMouseEvents);
	_guessTheCategoryCheckbox->setFocusPolicy(Qt::NoFocus);
	infoLayout->addWidget(_guessTheCategoryCheckbox, Qt::AlignHCenter);
	infoLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
	categoryLayout->addItem(infoLayout, 1, 2, Qt::AlignHCenter);
	descriptionLayout->addItem(categoryLayout);

	/** Buttons */
	QPushButton* selectBtn = new QPushButton("Select Quiz");
	selectBtn->setObjectName("menuButton");
	connect(selectBtn, SIGNAL(released()), this, SLOT(quizSelected()));
	buttonLayout->addWidget(selectBtn);

	QPushButton* settingsBtn = new QPushButton("Settings");
	settingsBtn->setObjectName("menuButton");
	connect(settingsBtn, SIGNAL(released()), this, SLOT(openSettingsDialog()));
	buttonLayout->addWidget(settingsBtn);

	QPushButton* quitBtn = new QPushButton("Quit");
	quitBtn->setObjectName("menuButton");
	connect(quitBtn, SIGNAL(released()), this, SLOT(quit()));
	buttonLayout->addWidget(quitBtn);

	/** Add Widgets */
	mainlayout->addItem(quizSelectionLayout, 0, 0);
	mainlayout->addItem(descriptionLayout, 0, 1);
	mainlayout->addItem(buttonLayout, 1, 0, 1, 3);

	/** Set Layout */
	setLayout(mainlayout);
	setContentsMargins(20, 20, 20, 20);

	/** Set Selected Quiz */
	if ( !_quizPreviews.empty() ) {
		_quizSelectionList->setCurrentRow(0);
	}
}

void MusicQuiz::QuizSelector::selectionClicked()
{
	/** Sanity Check */
	if ( _quizPreviews.empty() ) {
		LOG_ERROR("Can not update QuizSelection no previews loaded.")
			return;
	}

	if ( _descriptionText == nullptr || _quizSelectionList == nullptr ) {
		LOG_ERROR("Can not update QuizSelection item are nullptr.")
			return;
	}

	/** Current Index */
	int currentIndex = _quizSelectionList->currentRow();
	if ( currentIndex < 0 || static_cast<size_t>(currentIndex) >= _quizPreviews.size() ) {
		LOG_ERROR("Can not update QuizSelection index out of range.")
			return;
	}

	/** Update Description */
	_descriptionText->setText(QString::fromStdString(_quizPreviews[currentIndex].quizDescription));

	/** Update Info Checkboxes */
	_includeSongsCheckbox->setChecked(_quizPreviews[currentIndex].includeSongs);
	_includeVideosCheckbox->setChecked(_quizPreviews[currentIndex].includeVideos);
	_guessTheCategoryCheckbox->setChecked(_quizPreviews[currentIndex].guessTheCategory);

	/** Update Categories */
	std::stringstream ss("\n");
	std::vector<std::string> categories = _quizPreviews[currentIndex].categories;
	for ( size_t i = 0; i < categories.size(); ++i ) {
		if ( _quizPreviews[currentIndex].guessTheCategory ) {
			ss << i + 1 << ". Hidden\n\n";
		} else {
			ss << i + 1 << ". " << categories[i] << "\n\n";
		}
	}
	_categoryText->setText(QString::fromStdString(ss.str()));

	/** Row Cateogires */
	std::vector<std::string> rowCategories = _quizPreviews[currentIndex].rowCategories;
	if ( !rowCategories.empty() ) {
		ss.str("\n");
		for ( size_t i = 0; i < rowCategories.size(); ++i ) {
			ss << i + 1 << ". " << rowCategories[i] << "\n\n";
		}
		_rowCategoryText->setText(QString::fromStdString(ss.str()));
	} else {
		_rowCategoryText->clear();
	}

}

void MusicQuiz::QuizSelector::quizSelected()
{
	/** Sanity Check */
	if ( _quizPreviews.empty() ) {
		LOG_ERROR("Can not select quiz no quizzes loaded.")
			return;
	}

	if ( _quizSelectionList == nullptr ) {
		LOG_ERROR("Can not select quiz item is nullptr.")
			return;
	}

	/** Current Index */
	int currentIndex = _quizSelectionList->currentRow();
	if ( currentIndex < 0 || static_cast<size_t>(currentIndex) >= _quizPreviews.size() ) {
		LOG_ERROR("Can not select quiz index out of range.")
			return;
	}

	/** Quiz Name */
	const QString quizName = QString::fromStdString(_quizPreviews[currentIndex].quizName);

	/** Quiz Name */
	const QString quizAuthor = QString::fromStdString(_quizPreviews[currentIndex].quizAuthor);

	/** Guess The Category */
	_settings.guessTheCategory = _quizPreviews[currentIndex].guessTheCategory;

	/** Popup Messagebox */
	QString msg = "Are you sure you want to select quiz '" + quizName + "'?";
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Select Quiz?", msg,
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

	if ( resBtn == QMessageBox::Yes ) {
		emit quizSelectedSignal(currentIndex, quizName, quizAuthor, _settings);
	}
}

void MusicQuiz::QuizSelector::openSettingsDialog()
{
	MusicQuiz::QuizSettingsDialog* settingsDialog = new MusicQuiz::QuizSettingsDialog(_settings, this);
	settingsDialog->setObjectName("settingsDialog");
	connect(settingsDialog, SIGNAL(settingsUpdated(const MusicQuiz::QuizSettings&)), this, SLOT(updateSettings(const MusicQuiz::QuizSettings&)));
	settingsDialog->exec();
}

void MusicQuiz::QuizSelector::updateSettings(const MusicQuiz::QuizSettings& settings)
{
	_settings = settings;
}

void MusicQuiz::QuizSelector::quit()
{
	closeWindow();
}

void MusicQuiz::QuizSelector::closeEvent(QCloseEvent* event)
{
	if ( _quizClosed || closeWindow() ) {
		event->accept();
	} else {
		event->ignore();
	}
}

bool MusicQuiz::QuizSelector::closeWindow()
{
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Close Music Quiz?", "Are you sure you want to close the quiz?",
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

	if ( resBtn == QMessageBox::Yes ) {
		_quizClosed = true;
		emit quitSignal();
		return true;
	}

	return false;
}

void MusicQuiz::QuizSelector::keyPressEvent(QKeyEvent* event)
{
	switch ( event->key() )
	{
	case Qt::Key_Escape:
		closeWindow();
		break;
	default:
		QWidget::keyPressEvent(event);
		break;
	}
}