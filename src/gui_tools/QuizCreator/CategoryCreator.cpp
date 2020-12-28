#include "CategoryCreator.hpp"

#include <filesystem>

#include <QLabel>
#include <QRegExp>
#include <QString>
#include <QGridLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QTableWidgetItem>

#include "common/Configuration.hpp"
#include "common/Log.hpp"

#include "gui_tools/QuizCreator/EntryCreator.hpp"


MusicQuiz::CategoryCreator::CategoryCreator(const QString& name, const media::AudioPlayer::Ptr& audioPlayer, const common::Configuration& config, QWidget* parent) :
	QWidget(parent), _categoryName(name), _audioPlayer(audioPlayer), _config(config)
{
	/** Create Layout */
	createLayout();
}

MusicQuiz::CategoryCreator::CategoryCreator(const boost::property_tree::ptree &tree, const media::AudioPlayer::Ptr& audioPlayer, 
	const common::Configuration& config, bool skipEntries, QWidget* parent) :
	QWidget(parent), 
	_categoryName(QString::fromStdString(tree.get<std::string>("<xmlattr>.name"))),
	_audioPlayer(audioPlayer),
	_config(config)
{
	createLayout();
	if(!skipEntries) {
		std::vector< MusicQuiz::EntryCreator* > categorieEntries;
		boost::property_tree::ptree::const_iterator it = tree.begin();
		for ( ; it != tree.end(); ++it ) {
			try {
				if ( it->first == "QuizEntry" ) {
					categorieEntries.push_back(new MusicQuiz::EntryCreator(it->second, _audioPlayer, _config, this));
				}
			} catch ( ... ) {}
		}
		setEntries(categorieEntries);
	}
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

	_entriesTable = new QTableWidget(0, 5);
	_entriesTable->setObjectName("quizCreatorTable");
	_entriesTable->setDragDropMode(QAbstractItemView::InternalMove);
	_entriesTable->setSelectionMode(QAbstractItemView::NoSelection);
	_entriesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	_entriesTable->setStyleSheet("QHeaderView { qproperty-defaultAlignment: AlignCenter; }");
	_entriesTable->horizontalHeader()->setVisible(false);
	_entriesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	_entriesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	_entriesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	_entriesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
	_entriesTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
	_entriesTable->verticalHeader()->setFixedWidth(40);
	_entriesTable->verticalHeader()->setSectionsMovable(false); // \todo set this to true to enable dragging.
	_entriesTable->verticalHeader()->setDefaultSectionSize(40);
	_entriesTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	setupTabLayout->addWidget(_entriesTable, ++row, 0, 1, 2);

	/** Set Layout */
	setLayout(mainlayout);
}

void MusicQuiz::CategoryCreator::addEntry(MusicQuiz::EntryCreator* entry, int entryIndex)
{
	QString entryNameStr;

	/** Get Number of Entries */

	if(entryIndex < 0) {
	 	entryIndex = _entriesTable->rowCount();
	}
	if(entry == nullptr) {
		entryNameStr = "Entry " + QString::number(entryIndex + 1);
		const int points = (entryIndex + 1) * 100;
		entry = new MusicQuiz::EntryCreator(entryNameStr, points, _audioPlayer, _config, this);
	} else {
		entryNameStr = entry->getName();
	}
	/** Sanity Check */
	if ( _entriesTable == nullptr ) {
		return;
	}

	/** Insert New Entry */
	_entriesTable->insertRow(entryIndex);

	/** Add Line Edit */
	QLineEdit* entryName = new QLineEdit(entryNameStr);
	QRegExp re("^[a-zA-Z0-9\\_\\.\\,\\-\\s\\'\\+\\^\\(\\)]{1,50}");
	QRegExpValidator* validator = new QRegExpValidator(re);
	entryName->setValidator(validator);
	entryName->setObjectName("quizCreatorCategoryLineEdit");
	entryName->setProperty("index", entryIndex);
	connect(entryName, SIGNAL(textChanged(const QString&)), this, SLOT(updateEntryTabName(const QString&)));
	_entriesTable->setCellWidget(entryIndex, 0, entryName);

	/** Add Edit Category Button */
	addButtonToTable("quizCreatorEditBtn", entryIndex, 1, &MusicQuiz::CategoryCreator::editEntry);

	/** Add Remove Entry Button */
	addButtonToTable("quizCreatorRemoveBtn", entryIndex, 2, &MusicQuiz::CategoryCreator::removeEntry);

	/** Add Move Up Button */
	addButtonToTable("quizCreatorUpBtn", entryIndex, 3, &MusicQuiz::CategoryCreator::moveEntryUp);

	/** Add Move Down Button */
	addButtonToTable("quizCreatorDownBtn", entryIndex, 4, &MusicQuiz::CategoryCreator::moveEntryDown);

	/** Add Tab */
	_entries.insert(_entries.begin() + entryIndex, entry);
	_tabWidget->insertTab(entryIndex + 1, entry, entryNameStr);

	updateIndices();
}

void MusicQuiz::CategoryCreator::addButtonToTable(const QString& objectName, int row, int column, void (MusicQuiz::CategoryCreator::*releasedCallback)(void))
{
	QPushButton* btn = new QPushButton;
	btn->setObjectName(objectName);
	btn->setProperty("index", row);

	QHBoxLayout* btnLayout = new QHBoxLayout;
	btnLayout->addWidget(btn, Qt::AlignCenter);

	QWidget* layoutWidget = new QWidget;
	layoutWidget->setLayout(btnLayout);
	_entriesTable->setCellWidget(row, column, layoutWidget);

	if(releasedCallback) {
		connect(btn, &QPushButton::released, this, releasedCallback);
	}
}

void MusicQuiz::CategoryCreator::editEntry()
{
	/** Sanity Check */
	if ( _entriesTable == nullptr ) {
		return;
	}

	/** Get Number of Categories */
	const int entryCount = _entriesTable->rowCount();

	/** Get Index */
	const int index = getSenderIdx();
	if ( index >= _tabWidget->count() || index >= entryCount ) {
		return;
	}

	/** Go to the Tab */
	_tabWidget->setCurrentIndex(index + 1);
}

void MusicQuiz::CategoryCreator::removeEntry()
{
	if ( _entriesTable == nullptr ) {
		return;
	}

	const int index = getSenderIdx();
	if(index >= _entriesTable->rowCount()) {
		return;
	}

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

	removeEntry(index);
}
void MusicQuiz::CategoryCreator::removeEntry(int index)
{
	/** Sanity Check */
	if ( _entriesTable == nullptr ) {
		return;
	}

	if ( index >= _tabWidget->count() || index >= _entriesTable->rowCount() || index < 0) {
		return;
	}

	/** Delete Entry in table */
	_entriesTable->removeRow(index);
	_entries.erase(_entries.begin() + index);

	/** Delete Tab */
	_tabWidget->removeTab(index + 1);

	updateIndices(index);
}

void MusicQuiz::CategoryCreator::updateIndices(int deleteIndex)
{
	for ( int i = 0; i < _entriesTable->rowCount(); ++i ) {
		/** Line Edit */
		QLineEdit* tmpLineEdit = qobject_cast<QLineEdit*>(_entriesTable->cellWidget(i, 0));
		if ( tmpLineEdit != nullptr ) {
			tmpLineEdit->setProperty("index", i);
		}

		/** Button */
		QList<QPushButton*> buttons = _entriesTable->cellWidget(i, 1)->findChildren<QPushButton*>() + 
									  _entriesTable->cellWidget(i, 2)->findChildren<QPushButton*>() + 
									  _entriesTable->cellWidget(i, 3)->findChildren<QPushButton*>();
		for ( QPushButton* tmpButton : buttons ) {
			if ( tmpButton != nullptr ) {
				if ( tmpButton->property("index").toInt() == deleteIndex ) {
					tmpButton = nullptr;
					delete tmpButton;
				} else {
					tmpButton->setProperty("index", i);
				}
			}
		}
	}
}

void MusicQuiz::CategoryCreator::moveEntryUp()
{
	const int idx = getSenderIdx();
	swapEntries(idx, idx -1);
}

void MusicQuiz::CategoryCreator::moveEntryDown()
{
	const int idx = getSenderIdx();
	swapEntries(idx, idx + 1);
}


int MusicQuiz::CategoryCreator::getSenderIdx() const
{
	const QPushButton* const button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr ) {
		return -1;
	}

	return button->property("index").toInt();
}

void MusicQuiz::CategoryCreator::swapEntries(int firstIdx, int secondIdx)
{
	/** Sanity Check */
	if ( _entriesTable == nullptr ) {
		return;
	}

	std::vector<int> indexes {firstIdx, secondIdx};
	for(auto idx : indexes) {
		if ( idx < 0                          ||
			 idx >= _tabWidget->count()       || 
			 idx >= _entriesTable->rowCount() || 
			 idx >= static_cast<int>(_entries.size()) ) {
			return; 
		}
	}
	if(firstIdx == secondIdx) {
		return;
	}
	if(firstIdx > secondIdx) {
		std::swap(firstIdx, secondIdx);
	}

	auto firstEntry = _entries[firstIdx];
	auto secondEntry = _entries[secondIdx];

	removeEntry(secondIdx);
	removeEntry(firstIdx);
	addEntry(secondEntry, firstIdx);
	addEntry(firstEntry, secondIdx);
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

	_entries.clear();

	/** Add Entries to Table */
	for ( auto entry : entries) {
		addEntry(entry);
	}
}

const std::vector< MusicQuiz::EntryCreator* > MusicQuiz::CategoryCreator::getEntries() const
{
	return _entries;
}

bool MusicQuiz::CategoryCreator::areEntryNamesUnique() const
{
	for(size_t i = 0; i < _entries.size(); ++i) {
		for ( size_t j = 0; j < _entries.size(); ++j ) {
			if ( i != j && _entries[i]->getName().toStdString() == _entries[j]->getName().toStdString() ) {
				return false;
			}
		}
	}
	return true;
}

boost::property_tree::ptree MusicQuiz::CategoryCreator::saveToXml(const std::string savePath, const std::string& xmlPath)
{
	const std::string name = getName().toStdString();
	if ( name.empty() ) {
		throw std::runtime_error("Failed to save quiz. All categories must have a name");
	}
	if(!areEntryNamesUnique()) {
		throw std::runtime_error("Failed to save quiz. " + name + ": All entires in a category must have a unique name.");
	}

	boost::property_tree::ptree tree;
	tree.put("<xmlattr>.name", name);

	std::error_code filesystem_error;
	std::filesystem::create_directory(savePath + "/" + name, filesystem_error);

	if ( filesystem_error ) {
		throw std::runtime_error("Failed to create directory to save the category files in.");
	}
	for ( auto entry : _entries) {
		if(entry->getName().toStdString().empty()) {
			throw std::runtime_error("Failed to save quiz. " + name + ": All entries needs to have a name.");
		}
		tree.add_child("QuizEntry", entry->toXml(savePath + "/" + name, xmlPath + "/" + name));
	}
	return tree;
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