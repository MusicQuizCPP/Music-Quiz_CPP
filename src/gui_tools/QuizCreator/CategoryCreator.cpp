#include "CategoryCreator.hpp"

#include <QLabel>
#include <QRegExp>
#include <QString>
#include <QGridLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QTableWidgetItem>

#include "common/Log.hpp"
#include "common/Configuration.hpp"

#include "gui_tools/QuizCreator/EntryCreator.hpp"
#include "gui_tools/QuizCreator/LoadQuizDialog.hpp"
#include "gui_tools/QuizCreator/LoadCategoryDialog.hpp"


MusicQuiz::CategoryCreator::CategoryCreator(const QString& name, const media::AudioPlayer::Ptr& audioPlayer, const common::Configuration& config, QWidget* parent) :
	QWidget(parent), _categoryName(name), _audioPlayer(audioPlayer), _config(config)
{
	/** Create Layout */
	createLayout();
}

void MusicQuiz::CategoryCreator::createLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;

	/** Vertical Tab Bar */
	_tabWidget = new QExtensions::QTabWidgetExtender;
	_tabWidget->tabBar()->setObjectName("_verticalTabBar");
	mainlayout->addWidget(_tabWidget);

	/** Setup Tab */
	QWidget* setupTab = new QWidget;
	QGridLayout* setupTabLayout = new QGridLayout;
	setupTab->setLayout(setupTabLayout);
	_tabWidget->addTab(setupTab, "Setup");
	int row = 0;

	/** Setup Tab - Category Name */
	_categoryNameLabel = new QLabel(_categoryName);
	_categoryNameLabel->setObjectName("quizCreatorLabel");
	setupTabLayout->addWidget(_categoryNameLabel, ++row, 0, 1, 2, Qt::AlignCenter);

	/** Setup Tab - Entries */
	QLabel* label = new QLabel("Entries:");
	label->setObjectName("quizCreatorLabel");
	setupTabLayout->addWidget(label, ++row, 0, 1, 1, Qt::AlignLeft);

	QPushButton* addEntryBtn = new QPushButton;
	addEntryBtn->setObjectName("quizCreatorAddBtn");
	addEntryBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	connect(addEntryBtn, SIGNAL(released()), this, SLOT(addEntry()));
	setupTabLayout->addWidget(addEntryBtn, row, 1, 1, 1, Qt::AlignRight);

	_entriesTable = new QTableWidget(0, 3);
	_entriesTable->setObjectName("quizCreatorTable");
	_entriesTable->setDragDropMode(QAbstractItemView::InternalMove);
	_entriesTable->setSelectionMode(QAbstractItemView::SingleSelection);
	_entriesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	_entriesTable->setStyleSheet("QHeaderView { qproperty-defaultAlignment: AlignCenter; }");
	_entriesTable->horizontalHeader()->setVisible(false);
	_entriesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	_entriesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	_entriesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	_entriesTable->verticalHeader()->setFixedWidth(40);
	_entriesTable->verticalHeader()->setSectionsMovable(false); // \todo set this to true to enable dragging.
	_entriesTable->verticalHeader()->setDefaultSectionSize(40);
	_entriesTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	setupTabLayout->addWidget(_entriesTable, ++row, 0, 1, 2);

	/** Load Category Button */
	QPushButton* loadCategoryBtn = new QPushButton("Load Category");
	loadCategoryBtn->setObjectName("quizCreatorLoadCategoryBtn");
	loadCategoryBtn->setProperty("loadOption", "category");
	connect(loadCategoryBtn, SIGNAL(released()), this, SLOT(openSelectQuizDialog()));
	setupTabLayout->addWidget(loadCategoryBtn, ++row, 1, 1, 1, Qt::AlignRight);

	/** Set Layout */
	setLayout(mainlayout);
}

void MusicQuiz::CategoryCreator::addEntry()
{
	/** Sanity Check */
	if ( _entriesTable == nullptr ) {
		return;
	}

	/** Get Number of Entries */
	const unsigned int entryCount = _entriesTable->rowCount();

	/** Insert New Entry */
	_entriesTable->insertRow(entryCount);

	/** Add Line Edit */
	const QString entryNameStr = "Entry " + QString::number(entryCount + 1);
	QLineEdit* entryName = new QLineEdit(entryNameStr);
	QRegExp re("^[a-zA-Z0-9\\_\\.\\,\\-\\s\\'\\+\\^\\(\\)]{1,50}");
	QRegExpValidator* validator = new QRegExpValidator(re);
	entryName->setValidator(validator);
	entryName->setObjectName("quizCreatorCategoryLineEdit");
	entryName->setProperty("index", entryCount);
	connect(entryName, SIGNAL(textChanged(const QString&)), this, SLOT(updateEntryTabName(const QString&)));
	_entriesTable->setCellWidget(entryCount, 0, entryName);

	/** Add Edit Category Button */
	QPushButton* editBtn = new QPushButton;
	editBtn->setObjectName("quizCreatorEditBtn");
	editBtn->setProperty("index", entryCount);
	connect(editBtn, SIGNAL(released()), this, SLOT(editEntry()));

	QHBoxLayout* btnLayout = new QHBoxLayout;
	btnLayout->addWidget(editBtn, Qt::AlignCenter);

	QWidget* layoutWidget = new QWidget;
	layoutWidget->setLayout(btnLayout);
	_entriesTable->setCellWidget(entryCount, 1, layoutWidget);

	/** Add Remove Entry Button */
	QPushButton* removeBtn = new QPushButton;
	removeBtn->setObjectName("quizCreatorRemoveBtn");
	removeBtn->setProperty("index", entryCount);
	connect(removeBtn, SIGNAL(released()), this, SLOT(removeEntry()));

	btnLayout = new QHBoxLayout;
	btnLayout->addWidget(removeBtn, Qt::AlignCenter);

	layoutWidget = new QWidget;
	layoutWidget->setLayout(btnLayout);
	_entriesTable->setCellWidget(entryCount, 2, layoutWidget);

	/** Add Tab */
	const int points = (entryCount + 1) * 100;
	MusicQuiz::EntryCreator* entry = new MusicQuiz::EntryCreator(entryNameStr, points, _audioPlayer, _config, this);
	_entries.push_back(entry);
	_tabWidget->addTab(entry, entryNameStr);
}

void MusicQuiz::CategoryCreator::editEntry()
{
	/** Sanity Check */
	if ( _entriesTable == nullptr ) {
		return;
	}

	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr ) {
		return;
	}

	/** Get Number of Categories */
	const int entryCount = _entriesTable->rowCount();

	/** Get Index */
	const int index = button->property("index").toInt();
	if ( index >= _tabWidget->count() || index >= entryCount ) {
		return;
	}

	/** Go to the Tab */
	_tabWidget->setCurrentIndex(index + 1);
}

void MusicQuiz::CategoryCreator::removeEntry()
{
	/** Sanity Check */
	if ( _entriesTable == nullptr ) {
		return;
	}

	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr ) {
		return;
	}

	/** Get Number of Entries */
	const int entryCount = _entriesTable->rowCount();

	/** Get Index */
	const int index = button->property("index").toInt();
	if ( index >= _tabWidget->count() || index >= entryCount ) {
		return;
	}

	/** Get Entry Name */
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(_entriesTable->cellWidget(index, 0));
	if ( lineEdit == nullptr ) {
		return;
	}

	/** Popup to ensure the user wants to delete the entry */
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Delete Entry?", "Are you sure you want to delete entry '" + lineEdit->text() + "'?",
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
	if ( resBtn != QMessageBox::Yes ) {
		return;
	}

	/** Delete Entry in table */
	_entriesTable->removeRow(index);
	_entries.erase(_entries.begin() + index);

	/** Delete Tab */
	_tabWidget->removeTab(index + 1);

	/** Update Indices */
	for ( int i = 0; i < _entriesTable->rowCount(); ++i ) {
		/** Line Edit */
		QLineEdit* tmpLineEdit = qobject_cast<QLineEdit*>(_entriesTable->cellWidget(i, 0));
		if ( tmpLineEdit != nullptr ) {
			tmpLineEdit->setProperty("index", i);
		}

		/** Button */
		QList<QPushButton*> buttons = _entriesTable->cellWidget(i, 1)->findChildren<QPushButton*>() + _entriesTable->cellWidget(i, 2)->findChildren<QPushButton*>();
		for ( QPushButton* tmpButton : buttons ) {
			if ( tmpButton != nullptr ) {
				if ( tmpButton->property("index").toInt() == index ) {
					tmpButton = nullptr;
					delete tmpButton;
				} else {
					tmpButton->setProperty("index", i);
				}
			}
		}
	}
}

void MusicQuiz::CategoryCreator::updateEntryTabName(const QString& str)
{
	/** Sanity Check */
	if ( _tabWidget == nullptr ) {
		return;
	}

	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
	if ( lineEdit == nullptr ) {
		return;
	}

	/** Get Index */
	const int index = lineEdit->property("index").toInt() + 1;
	if ( index >= _tabWidget->count() ) {
		return;
	}

	/** Set Tab Text */
	_tabWidget->setTabText(index, lineEdit->text());

	/** Update Category Name */
	MusicQuiz::EntryCreator* entryWidget = qobject_cast<MusicQuiz::EntryCreator*>(_tabWidget->widget(index));
	if ( entryWidget != nullptr ) {
		entryWidget->setName(str);
	}
}

void MusicQuiz::CategoryCreator::setName(const QString& name)
{
	/** Sanity Check */
	if ( _categoryNameLabel == nullptr ) {
		return;
	}

	/** Set Name */
	_categoryName = name;

	/** Update Label */
	_categoryNameLabel->setText(_categoryName);
}

const QString MusicQuiz::CategoryCreator::getName() const
{
	return _categoryName;
}

void MusicQuiz::CategoryCreator::setEntries(const std::vector< MusicQuiz::EntryCreator* >& entries)
{
	/** Sanity Check */
	if ( _entriesTable == nullptr ) {
		return;
	}

	/** Set Entries */
	_entries = entries;

	/** Add Entries to Table */
	for ( size_t i = 0; i < _entries.size(); ++i ) {
		/** Get Number of Entries */
		const unsigned int entryCount = _entriesTable->rowCount();

		/** Insert New Entry */
		_entriesTable->insertRow(entryCount);

		/** Add Line Edit */
		QLineEdit* entryName = new QLineEdit(_entries[i]->getName());
		QRegExp re("^[a-zA-Z0-9\\_\\.\\,\\-\\s\\'\\+\\^\\(\\)]{1,50}");
		QRegExpValidator* validator = new QRegExpValidator(re);
		entryName->setValidator(validator);
		entryName->setObjectName("quizCreatorCategoryLineEdit");
		entryName->setProperty("index", entryCount);
		connect(entryName, SIGNAL(textChanged(const QString&)), this, SLOT(updateEntryTabName(const QString&)));
		_entriesTable->setCellWidget(entryCount, 0, entryName);

		/** Add Edit Category Button */
		QPushButton* editBtn = new QPushButton;
		editBtn->setObjectName("quizCreatorEditBtn");
		editBtn->setProperty("index", entryCount);
		connect(editBtn, SIGNAL(released()), this, SLOT(editEntry()));

		QHBoxLayout* btnLayout = new QHBoxLayout;
		btnLayout->addWidget(editBtn, Qt::AlignCenter);

		QWidget* layoutWidget = new QWidget;
		layoutWidget->setLayout(btnLayout);
		_entriesTable->setCellWidget(entryCount, 1, layoutWidget);

		/** Add Remove Entry Button */
		QPushButton* removeBtn = new QPushButton;
		removeBtn->setObjectName("quizCreatorRemoveBtn");
		removeBtn->setProperty("index", entryCount);
		connect(removeBtn, SIGNAL(released()), this, SLOT(removeEntry()));

		btnLayout = new QHBoxLayout;
		btnLayout->addWidget(removeBtn, Qt::AlignCenter);

		layoutWidget = new QWidget;
		layoutWidget->setLayout(btnLayout);
		_entriesTable->setCellWidget(entryCount, 2, layoutWidget);

		/** Add Tab */
		_tabWidget->addTab(_entries[i], _entries[i]->getName());
	}
}

const std::vector< MusicQuiz::EntryCreator* > MusicQuiz::CategoryCreator::getEntries() const
{
	return _entries;
}

void MusicQuiz::CategoryCreator::clearEntries()
{
	/** Delete Entries */
	for ( size_t i = 0; i < _entries.size(); ++i ) {
		_entries[i] = nullptr;
		delete _entries[i];
	}

	/** Clear Vector */
	_entries.clear();
}

void MusicQuiz::CategoryCreator::openSelectQuizDialog()
{
	/** Create Dialog */
	MusicQuiz::LoadQuizDialog* loadQuizDialog = new MusicQuiz::LoadQuizDialog(_config, this);
	loadQuizDialog->setAttribute(Qt::WA_DeleteOnClose);

	/** Connect Signal */
	connect(loadQuizDialog, &MusicQuiz::LoadQuizDialog::loadSignal, this, &MusicQuiz::CategoryCreator::openSelectCategoryDialog);

	/** Open Dialog */
	loadQuizDialog->show();
}

void MusicQuiz::CategoryCreator::openSelectCategoryDialog(const std::string& quizName)
{
	/** Create Dialog */
	MusicQuiz::LoadCategoryDialog* loadQuizDialog = new MusicQuiz::LoadCategoryDialog(quizName, _config, this);

	/** Connect Signal */
	connect(loadQuizDialog, &MusicQuiz::LoadCategoryDialog::loadSignal, this, &MusicQuiz::CategoryCreator::loadCategory);

	/** Open Dialog */
	loadQuizDialog->exec();
}

void MusicQuiz::CategoryCreator::loadCategory(const std::string& quizName, const std::string& categoryName)
{
	/** Sanity Check */
	if ( quizName.empty() || categoryName.empty() ) {
		return;
	}

	/** Popup to ensure the user wants to load the quiz */
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Load Category?", "Are you sure you want to load the category? Any unsaved progress will be lost!",
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
	if ( resBtn == QMessageBox::No ) {
		return;
	}

	/** Load Quiz */
	LOG_INFO("Loading Category '" << categoryName << "' from quiz '" << quizName << "'.");
	MusicQuiz::QuizCategory* categoryData;
	try {
		categoryData = MusicQuiz::util::QuizLoader::loadQuizCategory(quizName, categoryName, _audioPlayer, _videoPlayer, _config, this);
	} catch ( const exception& err ) {
		QMessageBox::warning(this, "Info", "Failed to load quiz. " + QString::fromStdString(err.what()));
		return;
	} catch ( ... ) {
		QMessageBox::warning(this, "Info", "Failed to load quiz.");
		return;
	}

	loadQuizCategory(const std::string & quizName, const std::string & categoryName, const media::AudioPlayer::Ptr & audioPlayer,
		const media::VideoPlayer::Ptr & videoPlayer, const common::Configuration & config)


	/** Remove Tabs */
	while ( _tabWidget->count() > 1 ) {
		_tabWidget->removeTab(1);
	}

	/** Clear Categories */

}