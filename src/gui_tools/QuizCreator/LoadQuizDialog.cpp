#include "LoadQuizDialog.hpp"

#include <algorithm>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QRadioButton>

#include "util/QuizLoader.hpp"

void MusicQuiz::LoadQuizDialog::updateTable()
{
	/** Sanity Check */
	if ( _table == nullptr ) {
		return;
	}

	/** Clear table */
	_table->setRowCount(0);

	/** Get List of files */
	_quizList = MusicQuiz::util::QuizLoader::getListOfQuizzes(_config);

	/** Update Table */
	for ( unsigned int i = 0; i < _quizList.size(); ++i ) {
		/** Add Row */
		const int row = _table->rowCount();
		_table->insertRow(row);

		/** Quiz Name */
		std::replace(_quizList[i].begin(), _quizList[i].end(), '\\', '/');
		std::string quizName = _quizList[i];

		quizName = quizName.substr(quizName.find_last_of("/") + 1);
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
		_table->setCellWidget(row, 0, btnWidget);
	}
}

void MusicQuiz::LoadQuizDialog::load()
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
	emit loadSignal(_quizList[idx]);

	/** Close Dialog */
	close();
}