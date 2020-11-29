#include "QuizCreator.hpp"

#include <QList>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QGuiApplication>
#include <QScreen>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "common/Log.hpp"
#include "common/TimeUtil.hpp"
#include "common/Configuration.hpp"

#include "util/QuizSettings.hpp"

#include "gui_tools/widgets/QuizTeam.hpp"
#include "gui_tools/widgets/QuizBoard.hpp"
#include "gui_tools/widgets/QuizFactory.hpp"
#include "gui_tools/QuizCreator/EntryCreator.hpp"
#include "gui_tools/QuizCreator/LoadQuizDialog.hpp"
#include "gui_tools/QuizCreator/LoadCategoryDialog.hpp"

#include "gui_tools/QuizCreator/CategoryCreator.hpp"

using namespace std;

MusicQuiz::QuizCreator::QuizCreator(const common::Configuration& config, QWidget* parent) :
	QDialog(parent), _config(config)
{
	/** Set Window Flags */
	setWindowFlags(windowFlags() | Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);

	/** Set Window Title */
	setWindowTitle("Quiz Creator");

	/** Set Object Name */
	setObjectName("QuizCreator");

	/** Set Size */
	const int width = 1000;
	const int height = 800;
	if ( parent == nullptr ) {
		resize(width, height);
	} else {
		setGeometry(parent->x() + parent->width() / 2 - width / 2, parent->y() + parent->height() / 2 - height / 2, width, height);
	}

	/** Create Audio Player */
	_audioPlayer = make_shared<media::AudioPlayer>();

	/** Create Video Player */
	_videoPlayer = make_shared<media::VideoPlayer>();
	_videoPlayer->setWindowFlags(windowFlags() | Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);

	/** Set Video Player Size */
	const QRect screenRec = QGuiApplication::primaryScreen()->geometry();
	_videoPlayer->setMinimumSize(QSize(screenRec.width() / 4, screenRec.height() / 4));
	_videoPlayer->resize(QSize(screenRec.width() / 4, screenRec.height() / 4));

	/** Create Layout */
	createLayout();
}

void MusicQuiz::QuizCreator::createLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;

	/** Tab Widget */
	_tabWidget = new QTabWidget;
	mainlayout->addWidget(_tabWidget, 0, 0, 1, 5);

	/** Setup Tab */
	QWidget* setupTab = new QWidget;
	QGridLayout* setupTabLayout = new QGridLayout;
	setupTab->setLayout(setupTabLayout);
	_tabWidget->addTab(setupTab, "Setup");
	int row = 0;

	/** Setup Tab - Quiz Name */
	QLabel* label = new QLabel("Quiz Name:");
	label->setObjectName("quizCreatorLabel");
	setupTabLayout->addWidget(label, ++row, 0, 1, 2, Qt::AlignLeft);

	_quizNameLineEdit = new QLineEdit;
	QRegExp re("^[a-zA-Z0-9\\_\\.\\,\\-\\s\\'\\+\\^\\(\\)]{1,50}");
	QRegExpValidator* validator = new QRegExpValidator(re);
	_quizNameLineEdit->setValidator(validator);
	_quizNameLineEdit->setObjectName("quizCreatorLineEdit");
	setupTabLayout->addWidget(_quizNameLineEdit, ++row, 0, 1, 2);

	/** Setup Tab - Quiz Author */
	label = new QLabel("Quiz Author:");
	label->setObjectName("quizCreatorLabel");
	setupTabLayout->addWidget(label, ++row, 0, 1, 2, Qt::AlignLeft);

	_quizAuthorLineEdit = new QLineEdit;
	_quizAuthorLineEdit->setValidator(validator);
	_quizAuthorLineEdit->setObjectName("quizCreatorLineEdit");
	setupTabLayout->addWidget(_quizAuthorLineEdit, ++row, 0, 1, 2);

	/** Setup Tab - Quiz Description */
	label = new QLabel("Quiz Description:");
	label->setObjectName("quizCreatorLabel");
	setupTabLayout->addWidget(label, ++row, 0, 1, 2, Qt::AlignLeft);

	_quizDescriptionTextEdit = new QTextEdit;
	_quizDescriptionTextEdit->setObjectName("quizCreatorTextEdit");
	setupTabLayout->addWidget(_quizDescriptionTextEdit, ++row, 0, 1, 2);

	/** Setup Tab - Settings */
	label = new QLabel("Settings:");
	label->setObjectName("quizCreatorLabel");
	setupTabLayout->addWidget(label, ++row, 0, 1, 2, Qt::AlignLeft);

	_hiddenCategoriesCheckbox = new QCheckBox("Hidden Categories");
	_hiddenCategoriesCheckbox->setObjectName("quizCreatorCheckbox");
	setupTabLayout->addWidget(_hiddenCategoriesCheckbox, ++row, 0, 1, 2);

	/** Setup Tab - Categories */
	label = new QLabel("Categories:");
	label->setObjectName("quizCreatorLabel");
	setupTabLayout->addWidget(label, ++row, 0, 1, 1, Qt::AlignLeft);

	QPushButton* addCategoryBtn = new QPushButton;
	addCategoryBtn->setObjectName("quizCreatorAddBtn");
	addCategoryBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	connect(addCategoryBtn, SIGNAL(released()), this, SLOT(addCategory()));
	setupTabLayout->addWidget(addCategoryBtn, row, 1, 1, 1, Qt::AlignRight);

	_categoriesTable = new QTableWidget(0, 3, this);
	_categoriesTable->setObjectName("quizCreatorTable");
	_categoriesTable->setDragDropMode(QAbstractItemView::InternalMove);
	_categoriesTable->setSelectionMode(QAbstractItemView::SingleSelection);
	_categoriesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	_categoriesTable->setStyleSheet("QHeaderView { qproperty-defaultAlignment: AlignCenter; }");
	_categoriesTable->horizontalHeader()->setVisible(false);
	_categoriesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	_categoriesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	_categoriesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	_categoriesTable->verticalHeader()->setFixedWidth(40);
	_categoriesTable->verticalHeader()->setSectionsMovable(false); // \todo set this to true to enable dragging.
	_categoriesTable->verticalHeader()->setDefaultSectionSize(40);
	_categoriesTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	connect(_categoriesTable->verticalHeader(), SIGNAL(sectionMoved(int, int, int)), this, SLOT(categoryOrderChanged(int, int, int)));
	setupTabLayout->addWidget(_categoriesTable, ++row, 0, 1, 2);

	/** Setup Tab - Row Categories */
	label = new QLabel("Row Categories:");
	label->setObjectName("quizCreatorLabel");
	setupTabLayout->addWidget(label, ++row, 0, 1, 2, Qt::AlignLeft);

	QPushButton* addRowCategoryBtn = new QPushButton;
	addRowCategoryBtn->setObjectName("quizCreatorAddBtn");
	addRowCategoryBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	connect(addRowCategoryBtn, SIGNAL(released()), this, SLOT(addRowCategory()));
	setupTabLayout->addWidget(addRowCategoryBtn, row, 1, 1, 1, Qt::AlignRight);

	_rowCategoriesTable = new QTableWidget(0, 2);
	_rowCategoriesTable->setObjectName("quizCreatorTable");
	_rowCategoriesTable->setDragDropMode(QAbstractItemView::InternalMove);
	_rowCategoriesTable->setSelectionMode(QAbstractItemView::SingleSelection);
	_rowCategoriesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	_rowCategoriesTable->setStyleSheet("QHeaderView { qproperty-defaultAlignment: AlignCenter; }");
	_rowCategoriesTable->horizontalHeader()->setVisible(false);
	_rowCategoriesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	_rowCategoriesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	_rowCategoriesTable->verticalHeader()->setFixedWidth(40);
	_rowCategoriesTable->verticalHeader()->setSectionsMovable(false); // \todo set this to true to enable dragging.
	_rowCategoriesTable->verticalHeader()->setDefaultSectionSize(40);
	_rowCategoriesTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	setupTabLayout->addWidget(_rowCategoriesTable, ++row, 0, 1, 2);

	/** Bottom Buttons */
	QPushButton* previewQuizBtn = new QPushButton("Preview");
	previewQuizBtn->setObjectName("quizCreatorBtn");
	connect(previewQuizBtn, SIGNAL(released()), this, SLOT(previewQuiz()));
	mainlayout->addWidget(previewQuizBtn, 1, 0, 1, 1);

	QPushButton* saveQuizBtn = new QPushButton("Save Quiz");
	saveQuizBtn->setObjectName("quizCreatorBtn");
	connect(saveQuizBtn, SIGNAL(released()), this, SLOT(saveQuiz()));
	mainlayout->addWidget(saveQuizBtn, 1, 1, 1, 1);

	QPushButton* loadQuizBtn = new QPushButton("Load Quiz");
	loadQuizBtn->setObjectName("quizCreatorBtn");
	connect(loadQuizBtn, SIGNAL(released()), this, SLOT(openLoadQuizDialog()));
	mainlayout->addWidget(loadQuizBtn, 1, 2, 1, 1);

	QPushButton* loadQuizCategoryBtn = new QPushButton("Load Category");
	loadQuizCategoryBtn->setObjectName("quizCreatorBtn");
	connect(loadQuizCategoryBtn, SIGNAL(released()), this, SLOT(openLoadCategoryDialog()));
	mainlayout->addWidget(loadQuizCategoryBtn, 1, 3, 1, 1);

	QPushButton* quitCreatorBtn = new QPushButton("Quit");
	quitCreatorBtn->setObjectName("quizCreatorBtn");
	connect(quitCreatorBtn, SIGNAL(released()), this, SLOT(quitCreator()));
	mainlayout->addWidget(quitCreatorBtn, 1, 4, 1, 1);

	/** Set Layout */
	setLayout(mainlayout);
}

void MusicQuiz::QuizCreator::addCategory()
{
	/** Sanity Check */
	if ( _categoriesTable == nullptr ) {
		return;
	}

	/** Get Number of Categories */
	const unsigned int categoryCount = _categoriesTable->rowCount();

	/** Insert New Category */
	_categoriesTable->insertRow(categoryCount);

	/** Add Line Edit */
	const QString categoryNameStr = "Category " + QString::number(categoryCount + 1);
	QLineEdit* categoryName = new QLineEdit(categoryNameStr);
	QRegExp re("^[a-zA-Z0-9\\_\\.\\,\\-\\s\\'\\+\\^\\(\\)]{1,50}");
	QRegExpValidator* validator = new QRegExpValidator(re);
	categoryName->setValidator(validator);
	categoryName->setObjectName("quizCreatorCategoryLineEdit");
	categoryName->setProperty("index", categoryCount);
	connect(categoryName, SIGNAL(textChanged(const QString&)), this, SLOT(updateCategoryTabName(const QString&)));
	_categoriesTable->setCellWidget(categoryCount, 0, categoryName);

	/** Add Edit Category Button */
	QPushButton* editBtn = new QPushButton;
	editBtn->setObjectName("quizCreatorEditBtn");
	editBtn->setProperty("index", categoryCount);
	connect(editBtn, SIGNAL(released()), this, SLOT(editCategory()));

	QHBoxLayout* btnLayout = new QHBoxLayout;
	btnLayout->addWidget(editBtn, Qt::AlignCenter);

	QWidget* layoutWidget = new QWidget;
	layoutWidget->setLayout(btnLayout);
	_categoriesTable->setCellWidget(categoryCount, 1, layoutWidget);

	/** Add Remove Category Button */
	QPushButton* removeBtn = new QPushButton;
	removeBtn->setObjectName("quizCreatorRemoveBtn");
	removeBtn->setProperty("index", categoryCount);
	connect(removeBtn, SIGNAL(released()), this, SLOT(removeCategory()));

	btnLayout = new QHBoxLayout;
	btnLayout->addWidget(removeBtn, Qt::AlignCenter);

	layoutWidget = new QWidget;
	layoutWidget->setLayout(btnLayout);
	_categoriesTable->setCellWidget(categoryCount, 2, layoutWidget);

	/** Add Tab */
	MusicQuiz::CategoryCreator* category = new MusicQuiz::CategoryCreator(categoryNameStr, _audioPlayer, _config);
	_categories.push_back(category);
	_tabWidget->addTab(category, categoryNameStr);
}

void MusicQuiz::QuizCreator::addRowCategory()
{
	/** Sanity Check */
	if ( _rowCategoriesTable == nullptr ) {
		return;
	}

	/** Get Number of Categories */
	const unsigned int rowCategoryCount = _rowCategoriesTable->rowCount();

	/** Insert New Category */
	_rowCategoriesTable->insertRow(rowCategoryCount);

	/** Add Line Edit */
	QLineEdit* rowCategoryName = new QLineEdit("Row Category " + QString::number(rowCategoryCount + 1));
	QRegExp re("^[a-zA-Z0-9\\_\\.\\,\\-\\s\\'\\+\\^\\(\\)]{1,50}");
	QRegExpValidator* validator = new QRegExpValidator(re);
	rowCategoryName->setValidator(validator);
	rowCategoryName->setObjectName("quizCreatorCategoryLineEdit");
	_rowCategoriesTable->setCellWidget(rowCategoryCount, 0, rowCategoryName);

	/** Add Remove Category Button */
	QPushButton* removeBtn = new QPushButton;
	removeBtn->setObjectName("quizCreatorRemoveBtn");
	removeBtn->setProperty("index", rowCategoryCount);
	connect(removeBtn, SIGNAL(released()), this, SLOT(removeRowCategory()));

	QHBoxLayout* removeBtnLayout = new QHBoxLayout;
	removeBtnLayout->addWidget(removeBtn, Qt::AlignCenter);

	QWidget* layoutWidget = new QWidget;
	layoutWidget->setLayout(removeBtnLayout);
	_rowCategoriesTable->setCellWidget(rowCategoryCount, 1, layoutWidget);
}

void MusicQuiz::QuizCreator::editCategory()
{
	/** Sanity Check */
	if ( _categoriesTable == nullptr ) {
		return;
	}

	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr ) {
		return;
	}

	/** Get Number of Categories */
	const int categoryCount = _categoriesTable->rowCount();

	/** Get Index */
	const int index = button->property("index").toInt();
	if ( index >= _tabWidget->count() || index >= categoryCount ) {
		return;
	}

	/** Go to the Tab */
	_tabWidget->setCurrentIndex(index + 1);
}

void MusicQuiz::QuizCreator::removeCategory()
{
	/** Sanity Check */
	if ( _categoriesTable == nullptr ) {
		return;
	}

	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr ) {
		return;
	}

	/** Get Number of Categories */
	const int categoryCount = _categoriesTable->rowCount();

	/** Get Index */
	const int index = button->property("index").toInt();
	if ( index >= _tabWidget->count() || index >= categoryCount ) {
		return;
	}

	/** Get Category Name */
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(_categoriesTable->cellWidget(index, 0));
	if ( lineEdit == nullptr ) {
		return;
	}

	/** Popup to ensure the user wants to delete the category */
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Delete Category?", "Are you sure you want to delete category '" + lineEdit->text() + "'?",
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
	if ( resBtn != QMessageBox::Yes ) {
		return;
	}

	/** Delete Entries in the Category */
	_categories[index]->clearEntries();

	/** Delete Category in table */
	_categoriesTable->removeRow(index);
	_categories.erase(_categories.begin() + index);

	/** Delete Tab */
	_tabWidget->removeTab(index + 1);

	/** Update Indices */
	for ( int i = 0; i < _categoriesTable->rowCount(); ++i ) {
		/** Line Edit */
		QLineEdit* tmpLineEdit = qobject_cast<QLineEdit*>(_categoriesTable->cellWidget(i, 0));
		if ( tmpLineEdit != nullptr ) {
			tmpLineEdit->setProperty("index", i);
		}

		/** Button */
		QList<QPushButton*> buttons = _categoriesTable->cellWidget(i, 1)->findChildren<QPushButton*>() + _categoriesTable->cellWidget(i, 2)->findChildren<QPushButton*>();
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

void MusicQuiz::QuizCreator::removeRowCategory()
{
	/** Sanity Check */
	if ( _rowCategoriesTable == nullptr ) {
		return;
	}

	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr ) {
		return;
	}

	/** Get Number of Row Categories */
	const int rowCategoryCount = _rowCategoriesTable->rowCount();

	/** Get Index */
	const int index = button->property("index").toInt();
	if ( index >= rowCategoryCount ) {
		return;
	}

	/** Get Row Category Name */
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(_rowCategoriesTable->cellWidget(index, 0));
	if ( lineEdit == nullptr ) {
		return;
	}
	const QString rowCategoryName = lineEdit->text();

	/** Popup to ensure the user wants to delete the row category */
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Delete Row Category?", "Are you sure you want to delete row category '" + rowCategoryName + "'?",
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
	if ( resBtn != QMessageBox::Yes ) {
		return;
	}

	/** Delete Row Category in table */
	_rowCategoriesTable->removeRow(index);

	/** Update Button Indices */
	for ( int i = 0; i < _rowCategoriesTable->rowCount(); ++i ) {
		QList<QPushButton*> buttons = _rowCategoriesTable->cellWidget(i, 1)->findChildren<QPushButton*>();
		for ( QPushButton* tmpButton : buttons ) {
			if ( tmpButton != nullptr ) {
				tmpButton->setProperty("index", i);
			}
		}
	}
}

void MusicQuiz::QuizCreator::updateCategoryTabName(const QString& str)
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
	MusicQuiz::CategoryCreator* categoryWidget = qobject_cast<MusicQuiz::CategoryCreator*>(_tabWidget->widget(index));
	if ( categoryWidget != nullptr ) {
		categoryWidget->setName(str);
	}
}

const vector< QString > MusicQuiz::QuizCreator::getRowCategories() const
{
	/** Sanity Check */
	if ( _rowCategoriesTable == nullptr ) {
		return vector< QString >();
	}

	/** Get Number of Row Categories */
	const int rowCategoryCount = _rowCategoriesTable->rowCount();

	/** Get Row Category Name */
	vector< QString > rowCategories;
	for ( int i = 0; i < rowCategoryCount; ++i ) {
		QLineEdit* lineEdit = qobject_cast<QLineEdit*>(_rowCategoriesTable->cellWidget(i, 0));
		if ( lineEdit == nullptr ) {
			continue;
		}
		rowCategories.push_back(lineEdit->text());
	}

	/** Return */
	return rowCategories;
}

void MusicQuiz::QuizCreator::saveQuiz()
{
	/** Stop Song */
	_audioPlayer->stop();

	/** Stop Video */
	for ( size_t i = 0; i < _categories.size(); ++i ) {
		for ( size_t j = 0; j < _categories[i]->getEntries().size(); ++j ) {
			_categories[i]->getEntries()[j]->stop();
		}
	}

	/** Get Quiz Data */
	QuizData quizData(_config);

	/** Quiz Name */
	quizData.setName(_quizNameLineEdit->text().toStdString());

	/** Quiz Author */
	quizData.setAuthor(_quizAuthorLineEdit->text().toStdString());

	/** Quiz Description */
	quizData.setDescription(_quizDescriptionTextEdit->toPlainText().toStdString());

	/** Hidden Categoies */
	quizData.setGuessTheCategory(_hiddenCategoriesCheckbox->isChecked(), 500);

	/** Quiz Categories */
	quizData.setCategories(_categories);

	/** Quiz Row Categories */
	quizData.setRowCategories(getRowCategories());

	/** Save Quiz */
	if(quizData.doesQuizDirectoryExist()) {
		QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Overwrite Quiz?", "Quiz already exists, do you want to overwrite existing quiz?",
			QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
		if ( resBtn != QMessageBox::Yes ) {
			return;
		}
	}

	try {
		quizData.save();
		QMessageBox::information(this, "Info", "Quiz saves successfully.");
	} catch ( const exception& err ) {
		QMessageBox::warning(this, "Failed to Save Quiz", QString::fromStdString(err.what()));
	} catch ( ... ) {
		QMessageBox::warning(this, "Failed to Save Quiz", "Failed to save the quiz. Unkown Error.");
	}
}

void MusicQuiz::QuizCreator::openLoadQuizDialog()
{
	/** Create Dialog */
	MusicQuiz::LoadQuizDialog* dialog = new MusicQuiz::LoadQuizDialog(_config, this);

	dialog->updateTable();

	/** Connect Signal */
	connect(dialog, &MusicQuiz::LoadQuizDialog::loadSignal, this, &MusicQuiz::QuizCreator::loadQuiz);

	/** Open Dialog */
	dialog->exec();
}

void MusicQuiz::QuizCreator::openLoadCategoryDialog()
{
	/** Create Dialog */
	MusicQuiz::LoadCategoryDialog* dialog = new MusicQuiz::LoadCategoryDialog(_config, this);

	/** Connect Signal */
	connect(dialog, &MusicQuiz::LoadCategoryDialog::loadSignal, this, &MusicQuiz::QuizCreator::loadQuizCategory);

	dialog->updateTable();

	/** Open Dialog */
	dialog->exec();
}


void MusicQuiz::QuizCreator::loadQuiz(const string& quizName)
{
	/** Sanity Check */
	if ( quizName.empty() ) {
		return;
	}

	/** Popup to ensure the user wants to load the quiz */
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Load Quiz?", "Are you sure you want to load the quiz? Any unsaved progress will be lost!",
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
	if ( resBtn == QMessageBox::No ) {
		return;
	}

	/** Load Quiz */
	LOG_INFO("Loading quiz '" << quizName << "'");
	try {
		loadQuizData(QuizData(_config, quizName, _audioPlayer, this));
	} catch ( const exception& err ) {
		QMessageBox::warning(this, "Info", "Failed to load quiz. " + QString::fromStdString(err.what()));
		return;
	} catch ( ... ) {
		QMessageBox::warning(this, "Info", "Failed to load quiz.");
		return;
	}
}

void MusicQuiz::QuizCreator::loadQuizCategory(const string& quizName, const string& categoryName)
{
	/** Sanity Check */
	if ( quizName.empty() || categoryName.empty()) {
		return;
	}


	/** Load Quiz */
	LOG_INFO("Loading quiz '" << quizName << "'");
	try {
		QuizData quizData = QuizData(_config, quizName, _audioPlayer, this, false, regex("^" + categoryName + "$"));
		for(auto &category : quizData.getCategories()) {
			loadCategory(category);
		}
	} catch ( const exception& err ) {
		QMessageBox::warning(this, "Info", "Failed to load quiz. " + QString::fromStdString(err.what()));
		return;
	} catch ( ... ) {
		QMessageBox::warning(this, "Info", "Failed to load quiz.");
		return;
	}
}


void MusicQuiz::QuizCreator::loadQuizData(const QuizData& quizData)
{
	/** Remove Tabs */
	while ( _tabWidget->count() > 1 ) {
		_tabWidget->removeTab(1);
	}

	/** Clear Categories */
	for ( size_t i = 0; i < _categories.size(); ++i ) {
		_categories[i]->clearEntries();
		_categories[i] = nullptr;
		delete _categories[i];
	}
	_categories.clear();
	_categoriesTable->clear();
	_categoriesTable->clearContents();
	_categoriesTable->setRowCount(0);

	/** Clear Row Categories */
	_rowCategoriesTable->clear();
	_rowCategoriesTable->clearContents();
	_rowCategoriesTable->setRowCount(0);

	/** Set Name */
	if ( _quizNameLineEdit != nullptr ) {
		_quizNameLineEdit->setText(QString::fromStdString(quizData.getName()));
	}

	/** Set Author */
	if ( _quizAuthorLineEdit != nullptr ) {
		_quizAuthorLineEdit->setText(QString::fromStdString(quizData.getAuthor()));
	}

	/** Set Description */
	if ( _quizDescriptionTextEdit != nullptr ) {
		_quizDescriptionTextEdit->setText(QString::fromStdString(quizData.getDescription()));
	}

	/** Hidden Categories */
	if ( _hiddenCategoriesCheckbox != nullptr ) {
		_hiddenCategoriesCheckbox->setChecked(quizData.getGuessTheCategory());
	}

	/**Add Categories */
	_categories.clear();
	if ( _categoriesTable != nullptr ) {
		for( auto& category : quizData.getCategories() ) {
			loadCategory(category);
		}
	}

	/** Add Row Categories */
	if ( _rowCategoriesTable != nullptr ) {
		for ( const auto& rowCategory : quizData.getRowCategories() ) {
			loadRowCategory(rowCategory);
		}
	}
}

void MusicQuiz::QuizCreator::loadRowCategory(const std::string& rowCategory)
{
	/** Get Number of Row Categories */
	const unsigned int rowCategoryCount = _rowCategoriesTable->rowCount();

	/** Insert New Category */
	_rowCategoriesTable->insertRow(rowCategoryCount);

	/** Add Line Edit */
	QLineEdit* rowCategoryName = new QLineEdit(QString::fromStdString(rowCategory));
	rowCategoryName->setObjectName("quizCreatorCategoryLineEdit");
	_rowCategoriesTable->setCellWidget(rowCategoryCount, 0, rowCategoryName);

	/** Add Remove Category Button */
	QPushButton* removeBtn = new QPushButton;
	removeBtn->setObjectName("quizCreatorRemoveBtn");
	removeBtn->setProperty("index", rowCategoryCount);
	connect(removeBtn, SIGNAL(released()), this, SLOT(removeRowCategory()));

	QHBoxLayout* removeBtnLayout = new QHBoxLayout;
	removeBtnLayout->addWidget(removeBtn, Qt::AlignCenter);

	QWidget* layoutWidget = new QWidget;
	layoutWidget->setLayout(removeBtnLayout);
	_rowCategoriesTable->setCellWidget(rowCategoryCount, 1, layoutWidget);
}

void MusicQuiz::QuizCreator::loadCategory(MusicQuiz::CategoryCreator* category)
{
	_categories.push_back(category);

	/** Get Number of Entries */
	const unsigned int categoryCount = _categoriesTable->rowCount();

	/** Insert New Entry */
	_categoriesTable->insertRow(categoryCount);

	/** Add Line Edit */
	QLineEdit* categoryName = new QLineEdit(category->getName());
	categoryName->setObjectName("quizCreatorCategoryLineEdit");
	categoryName->setProperty("index", categoryCount);
	connect(categoryName, SIGNAL(textChanged(const QString&)), this, SLOT(updateCategoryTabName(const QString&)));
	_categoriesTable->setCellWidget(categoryCount, 0, categoryName);

	/** Add Edit Category Button */
	QPushButton* editBtn = new QPushButton;
	editBtn->setObjectName("quizCreatorEditBtn");
	editBtn->setProperty("index", categoryCount);
	connect(editBtn, SIGNAL(released()), this, SLOT(editCategory()));

	QHBoxLayout* btnLayout = new QHBoxLayout;
	btnLayout->addWidget(editBtn, Qt::AlignCenter);

	QWidget* layoutWidget = new QWidget;
	layoutWidget->setLayout(btnLayout);
	_categoriesTable->setCellWidget(categoryCount, 1, layoutWidget);

	/** Add Remove Entry Button */
	QPushButton* removeBtn = new QPushButton;
	removeBtn->setObjectName("quizCreatorRemoveBtn");
	removeBtn->setProperty("index", categoryCount);
	connect(removeBtn, SIGNAL(released()), this, SLOT(removeCategory()));

	btnLayout = new QHBoxLayout;
	btnLayout->addWidget(removeBtn, Qt::AlignCenter);

	layoutWidget = new QWidget;
	layoutWidget->setLayout(btnLayout);
	_categoriesTable->setCellWidget(categoryCount, 2, layoutWidget);

	/** Add Tab */
	_tabWidget->addTab(category, category->getName());
}

void MusicQuiz::QuizCreator::previewQuiz()
{
	/** Stop Song */
	_audioPlayer->stop();
	_videoPlayer->stop();

	/** Check that quiz have been saved */
	const string quizName = _quizNameLineEdit->text().toStdString();
	const string quizPath = _config.getQuizDataPath() + "/" + quizName + "/" + quizName + ".quiz.xml", tree;
	if ( !filesystem::exists(quizPath) ) {
		QMessageBox::information(nullptr, "Info", "Quiz must be saved before the preview can be shown.");
		return;
	}

	/** Dummy Settings */
	MusicQuiz::QuizSettings settings;
	settings.guessTheCategory = _hiddenCategoriesCheckbox->isChecked();

	/** Create Quiz Preview */
	try {
		_previewQuizBoard = MusicQuiz::QuizFactory::createQuiz(quizPath, settings, _audioPlayer, _videoPlayer, _config, {}, true, this);
		if ( _previewQuizBoard == nullptr ) {
			QMessageBox::warning(this, "Info", "Failed to preview quiz.");
			return;
		}

		/** Connect Signals */
		connect(_previewQuizBoard, SIGNAL(quitSignal()), this, SLOT(stopQuizPreview()));

		/** Start Preview */
		_previewQuizBoard->show();
	} catch ( const exception& err ) {
		QMessageBox::warning(this, "Info", "Failed to preview quiz. " + QString::fromStdString(err.what()));
		return;
	} catch ( ... ) {
		QMessageBox::warning(this, "Info", "Failed to preview quiz.");
		return;
	}
}

void MusicQuiz::QuizCreator::stopQuizPreview()
{
	/** Sanity Check */
	if ( _previewQuizBoard == nullptr ) {
		return;
	}

	/** Stop Audio */
	if ( _audioPlayer != nullptr ) {
		_audioPlayer->stop();
	}

	/** Stop Video */
	if ( _videoPlayer != nullptr ) {
		_videoPlayer->stop();
		_videoPlayer->hide();
	}

	/** Stop Quiz */
	_previewQuizBoard->hide();
	_previewQuizBoard->close();
	delete _previewQuizBoard;
	_previewQuizBoard = nullptr;
}

void MusicQuiz::QuizCreator::quitCreator()
{
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Close Quiz Creator?", "Are you sure you want to close the Quiz Creator?",
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

	if ( resBtn == QMessageBox::Yes ) {
		/** Call Destructor */
		QApplication::quit();
	}
}

void MusicQuiz::QuizCreator::categoryOrderChanged(const int, const int, const int)
{
	/** Sanity Check */
	if ( _categoriesTable == nullptr || _tabWidget == nullptr ) {
		return;
	}

	// \todo write the code for chaning the order of the tabs and the vector.
}