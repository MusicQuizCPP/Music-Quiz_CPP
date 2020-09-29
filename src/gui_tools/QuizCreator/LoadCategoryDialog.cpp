#include "LoadCategoryDialog.hpp"

#include <algorithm>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QRadioButton>

#include "common/Log.hpp"


MusicQuiz::LoadCategoryDialog::LoadCategoryDialog(const std::string& quizName, const common::Configuration& config, QWidget* parent) :
	QDialog(parent), _quizName(quizName), _config(config)
{
	/** Set Parameters */
	setModal(true);
	setWindowTitle("Load Category");
	setWindowFlags(windowFlags() | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	/** Create Widget Layout */
	makeWidgetLayout();

	/** Set Size */
	const int width = 350;
	const int height = 400;
	if ( parent == nullptr ) {
		resize(width, height);
	} else {
		setGeometry(parent->x() + parent->width() / 2 - width / 2, parent->y() + parent->height() / 2 - height / 2, width, height);
	}

	/** Update Table */
	updateTable();
}

void MusicQuiz::LoadCategoryDialog::makeWidgetLayout()
{
	/** Layouts */
	QGridLayout* mainLayout = new QGridLayout;
	mainLayout->setHorizontalSpacing(15);
	mainLayout->setVerticalSpacing(15);

	/** Jobs Table */
	_categoryTable = new QTableWidget(0, 1);
	_categoryTable->setStyleSheet("border: 0;");
	_categoryTable->setObjectName("quizCreatorLoaderTable");
	_categoryTable->setSelectionMode(QAbstractItemView::NoSelection);
	_categoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_categoryTable->setSelectionBehavior(QAbstractItemView::SelectItems);

	_categoryTable->horizontalHeader()->setStretchLastSection(true);
	_categoryTable->horizontalHeader()->setVisible(false);
	_categoryTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	_categoryTable->verticalHeader()->setDefaultSectionSize(55);
	_categoryTable->verticalHeader()->setVisible(false);
	mainLayout->addWidget(_categoryTable, 0, 0, 1, 2);

	/** Button Group */
	_buttonGroup = new QButtonGroup;

	/** Load Button */
	QPushButton* loadBtn = new QPushButton("Load");
	loadBtn->setObjectName("quizCreatorBtn");
	QObject::connect(loadBtn, SIGNAL(released()), this, SLOT(loadCategory()));
	mainLayout->addWidget(loadBtn, 1, 0);

	/** Close Button */
	QPushButton* closeBtn = new QPushButton("Close");
	closeBtn->setObjectName("quizCreatorBtn");
	QObject::connect(closeBtn, SIGNAL(released()), this, SLOT(close()));
	mainLayout->addWidget(closeBtn, 1, 1);

	/** Set Layout */
	setLayout(mainLayout);
}

void MusicQuiz::LoadCategoryDialog::updateTable()
{
	/** Sanity Check */
	if ( _categoryTable == nullptr ) {
		return;
	}

	/** Clear table */
	_categoryTable->setRowCount(0);

	/** Get List of Categories */
	_categoryList = MusicQuiz::util::QuizLoader::getListOfQuizCategories(_quizName, _config);

	/** Update Table */
	for ( unsigned int i = 0; i < _categoryList.size(); ++i ) {
		/** Add Row */
		const int row = _categoryTable->rowCount();
		_categoryTable->insertRow(row);

		/** Category Name */
		std::string categoryName = _categoryList[i];

		/** Radio Button */
		QWidget* btnWidget = new QWidget(this);
		QHBoxLayout* btnLayout = new QHBoxLayout(btnWidget);

		QRadioButton* btn = new QRadioButton(QString::fromStdString(categoryName));
		btn->setObjectName("quizCreatorRadioButton");
		btn->setProperty("index", i);
		if ( i == 0 ) {
			btn->setChecked(true);
		}
		_buttonGroup->addButton(btn, i);

		btnLayout->addWidget(btn, Qt::AlignCenter | Qt::AlignVCenter);
		btnWidget->setLayout(btnLayout);
		btnLayout->setAlignment(Qt::AlignCenter);
		_categoryTable->setCellWidget(row, 0, btnWidget);
	}
}

void MusicQuiz::LoadCategoryDialog::loadCategory()
{
	/** Sanity Check */
	if ( _buttonGroup == nullptr ) {
		return;
	}

	/** Get Index */
	QRadioButton* btn = qobject_cast<QRadioButton*>(_buttonGroup->checkedButton());
	if ( btn == nullptr ) {
		close();
		return;
	}
	const size_t idx = btn->property("index").toInt();

	/** Close Dialog */
	close();

	/** Emit Signal */
	emit loadSignal(_quizName, _categoryList[idx]);
}