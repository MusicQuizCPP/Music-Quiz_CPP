#include "LoadCategoryDialog.hpp"

#include <algorithm>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QRadioButton>

#include "util/QuizLoader.hpp"
#include "common/Log.hpp"

#include "QuizData.hpp"
#include "CategoryCreator.hpp"

void MusicQuiz::LoadCategoryDialog::updateTable()
{
    /** Sanity Check */
    if ( _table == nullptr ) {
        return;
    }

    /** Clear table */
    _table->setRowCount(0);

    /** Get List of quizes */
    std::vector<std::string> quizList = MusicQuiz::util::QuizLoader::getListOfQuizzes(_config);
    for(auto& quizName : quizList) {
        try {
            MusicQuiz::QuizData qdata(_config, quizName, nullptr, this, true);
            for(auto &category : qdata.getCategories()) {
                _categoryList.push_back(std::make_pair(quizName, category->getName().toStdString()));
                category->deleteLater();
            }
        }
        catch ( const std::exception& err ) {
            LOG_ERROR("Failed to load quiz. " + quizName + " " + err.what());
            return;
        } catch ( ... ) {
            LOG_ERROR("Failed to load quiz. " + quizName);
            return;
        }
    }

    /** Update Table */
    for ( unsigned int i = 0; i < _categoryList.size(); ++i ) {
        /** Add Row */

        /** Quiz Name */
        std::string quizName = _categoryList[i].first.substr(_categoryList[i].first.find_last_of("\\") + 1);
        const std::string fileExtension = ".quiz.xml";
        quizName.erase(quizName.find(fileExtension), fileExtension.length());

        const int row = _table->rowCount();
        _table->insertRow(row);

        /** Radio Button */
        QWidget* btnWidget = new QWidget(this);
        QHBoxLayout* btnLayout = new QHBoxLayout(btnWidget);

        QRadioButton* btn = new QRadioButton(QString::fromStdString(quizName + "::" + _categoryList[i].second));
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

void MusicQuiz::LoadCategoryDialog::load()
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
    std::string quizName = _categoryList[idx].first;
    std::string categoryName = _categoryList[idx].second;

    std::replace( quizName.begin(), quizName.end(), '\\', '/');
    emit loadSignal(quizName, categoryName);

    /** Close Dialog */
    close();
}