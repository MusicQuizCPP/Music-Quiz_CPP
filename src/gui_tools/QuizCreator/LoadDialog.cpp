#include "LoadDialog.hpp"

#include <algorithm>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QRadioButton>


MusicQuiz::LoadDialog::LoadDialog(QWidget* parent) :
    QDialog(parent)
{
    /** Set Parameters */
    setModal(true);
    setWindowTitle("Load Quiz");
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
}

void MusicQuiz::LoadDialog::makeWidgetLayout()
{
    /** Layouts */
    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->setHorizontalSpacing(15);
    mainLayout->setVerticalSpacing(15);

    /** Jobs Table */
    _table = new QTableWidget(0, 1);
    _table->setStyleSheet("border: 0;");
    _table->setObjectName("quizCreatorLoaderTable");
    _table->setSelectionMode(QAbstractItemView::NoSelection);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectItems);

    _table->horizontalHeader()->setStretchLastSection(true);
    _table->horizontalHeader()->setVisible(false);
    _table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    _table->verticalHeader()->setDefaultSectionSize(55);
    _table->verticalHeader()->setVisible(false);
    mainLayout->addWidget(_table, 0, 0, 1, 2);

    /** Button Groupe */
    _buttonGroup = new QButtonGroup;

    /** Export Button */
    QPushButton* loadBtn = new QPushButton("Load");
    loadBtn->setObjectName("quizCreatorBtn");
    QObject::connect(loadBtn, SIGNAL(released()), this, SLOT(load()));
    mainLayout->addWidget(loadBtn, 1, 0);

    /** Close Button */
    QPushButton* closeBtn = new QPushButton("Close");
    closeBtn->setObjectName("quizCreatorBtn");
    QObject::connect(closeBtn, SIGNAL(released()), this, SLOT(close()));
    mainLayout->addWidget(closeBtn, 1, 1);

    /** Set Layout */
    setLayout(mainLayout);
}