#include "LoadQuizDialog.hpp"

#include <algorithm>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QRadioButton>


MusicQuiz::LoadQuizDialog::LoadQuizDialog(const common::Configuration& config, QWidget* parent) :
	QDialog(parent), _config(config)
{
	/** Set Parameters */
	setModal(true);
	setWindowTitle("Load Quiz");
	setWindowFlags(windowFlags() | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	/** Create Widget Layout */
	makeWidgetLayout();

	/** Set Size */
	const size_t width = 350;
	const size_t height = 400;
	if ( parent == nullptr ) {
		resize(width, height);
	} else {
		setGeometry(parent->x() + parent->width() / 2 - width / 2, parent->y() + parent->height() / 2 - height / 2, width, height);
	}

	/** Update Table */
	updateTable();
}

void MusicQuiz::LoadQuizDialog::makeWidgetLayout()
{
	/** Layouts */
	QGridLayout* mainLayout = new QGridLayout;
	mainLayout->setHorizontalSpacing(15);
	mainLayout->setVerticalSpacing(15);

	/** Jobs Table */
	_quizTable = new QTableWidget(0, 1);
	_quizTable->setStyleSheet("border: 0;");
	_quizTable->setObjectName("quizCreatorLoaderTable");
	_quizTable->setSelectionMode(QAbstractItemView::NoSelection);
	_quizTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_quizTable->setSelectionBehavior(QAbstractItemView::SelectItems);

	_quizTable->horizontalHeader()->setStretchLastSection(true);
	_quizTable->horizontalHeader()->setVisible(false);
	_quizTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	_quizTable->verticalHeader()->setDefaultSectionSize(55);
	_quizTable->verticalHeader()->setVisible(false);
	mainLayout->addWidget(_quizTable, 0, 0, 1, 2);

	/** Button Groupe */
	_buttonGroup = new QButtonGroup;

	/** Export Button */
	QPushButton* loadBtn = new QPushButton("Load");
	loadBtn->setObjectName("quizCreatorBtn");
	QObject::connect(loadBtn, SIGNAL(released()), this, SLOT(loadQuiz()));
	mainLayout->addWidget(loadBtn, 1, 0);

	/** Close Button */
	QPushButton* closeBtn = new QPushButton("Close");
	closeBtn->setObjectName("quizCreatorBtn");
	QObject::connect(closeBtn, SIGNAL(released()), this, SLOT(close()));
	mainLayout->addWidget(closeBtn, 1, 1);

	/** Set Layout */
	setLayout(mainLayout);
}

void MusicQuiz::LoadQuizDialog::updateTable()
{
	/** Sanity Check */
	if ( _quizTable == nullptr ) {
		return;
	}

	/** Clear table */
	_quizTable->setRowCount(0);

	/** Get List of weldfiles */
	_quizList = MusicQuiz::util::QuizLoader::getListOfQuizzes(_config);

	/** Update Table */
	for ( unsigned int i = 0; i < _quizList.size(); ++i ) {
		/** Add Row */
		const int row = _quizTable->rowCount();
		_quizTable->insertRow(row);

		/** Quiz Name */
		std::string quizName = _quizList[i].substr(_quizList[i].find_last_of("\\") + 1);
		const std::string fileExtension = ".quiz.xml";
		quizName.erase(quizName.find(fileExtension), fileExtension.length());

		/** Radio Button */
		QWidget* btnWidget = new QWidget(this);
		QHBoxLayout* btnLayout = new QHBoxLayout(btnWidget);

		QRadioButton* btn = new QRadioButton(QString::fromStdString(quizName));
		btn->setObjectName("quizCreatorRadioButton");
		btn->setProperty("index", i);
		if ( i == 0 ) {
			btn->setChecked(true);
		}
		_buttonGroup->addButton(btn, i);

		btnLayout->addWidget(btn, Qt::AlignCenter | Qt::AlignVCenter);
		btnWidget->setLayout(btnLayout);
		btnLayout->setAlignment(Qt::AlignCenter);
		_quizTable->setCellWidget(row, 0, btnWidget);
	}
}

void MusicQuiz::LoadQuizDialog::loadQuiz()
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

	/** Emit Signal */
	std::replace(_quizList[idx].begin(), _quizList[idx].end(), '\\', '/');
	emit loadSignal(_quizList[idx]);

	/** Close Dialog */
	close();
}