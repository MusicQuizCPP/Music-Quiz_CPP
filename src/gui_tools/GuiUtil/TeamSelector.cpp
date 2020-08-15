#include "TeamSelector.hpp"

#include <random>
#include <sstream>
#include <stdexcept>

#include <QColor>
#include <QLabel>
#include <QWidget>
#include <QRegExp>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QListWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QMessageBox>
#include <QHeaderView>
#include <QListWidgetItem>

#include "common/Log.hpp"
#include "util/QuizSettings.hpp"
#include "gui_tools/widgets/QuizSettingsDialog.hpp"


MusicQuiz::TeamSelector::TeamSelector(QWidget* parent) :
	QDialog(parent)
{
	/** Set Window Flags */
	setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);

	/** Set Object Name */
	setObjectName("TeamSelector");

	/** Create Layout */
	createLayout();

	/** Set Fullscreen */
	showFullScreen();
}

void MusicQuiz::TeamSelector::createLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QGridLayout* selectionLayout = new QGridLayout;
	mainlayout->setHorizontalSpacing(20);
	mainlayout->setVerticalSpacing(20);
	selectionLayout->setHorizontalSpacing(20);
	selectionLayout->setVerticalSpacing(20);
	mainlayout->setColumnStretch(0, 1);
	mainlayout->setColumnStretch(1, 1);
	mainlayout->setColumnStretch(2, 1);
	int row = 0;

	/** Team Table */
	_teamTable = new QTableWidget(0, 3);
	_teamTable->setObjectName("teamTable");
	_teamTable->setFocusPolicy(Qt::NoFocus);
	_teamTable->setSelectionMode(QAbstractItemView::NoSelection);
	_teamTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_teamTable->setColumnWidth(0, 200);
	_teamTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	_teamTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	_teamTable->horizontalHeader()->setVisible(false);
	_teamTable->horizontalHeader()->setEnabled(false);
	_teamTable->verticalHeader()->setVisible(false);
	_teamTable->verticalHeader()->setDefaultSectionSize(100);

	/** Team Name */
	QLabel* teamNameLabel = new QLabel("Team Name:");
	teamNameLabel->setObjectName("teamInfoLabel");
	teamNameLabel->setAlignment(Qt::AlignLeft);
	selectionLayout->addWidget(teamNameLabel, 0, 0, Qt::AlignLeft);

	/** Team Line Edit */
	QRegExp re(".{1,14}");
	QRegExpValidator* validator = new QRegExpValidator(re);
	_teamNameLineEdit = new QLineEdit;
	_teamNameLineEdit->setValidator(validator);
	_teamNameLineEdit->setFocusPolicy(Qt::StrongFocus);
	_teamNameLineEdit->setObjectName("_teamLineEdit");
	_teamNameLineEdit->setAlignment(Qt::AlignCenter);
	selectionLayout->addWidget(_teamNameLineEdit, 1, 0);

	/** Team Color */
	QLabel* teamColorLabel = new QLabel("Team Color:");
	teamColorLabel->setObjectName("teamInfoLabel");
	teamColorLabel->setAlignment(Qt::AlignLeft);
	selectionLayout->addWidget(teamColorLabel, 0, 1, Qt::AlignLeft);
	teamColorChanged(QColor(5, 30, 190));

	/** Gradient Slider */
	_hueSlider = new ColorPicker::QHueSlider(this);
	_hueSlider->setStyleSheet("	border: 2px solid rgb(128, 128, 128); min-height: 100px;");
	connect(_hueSlider, SIGNAL(colorChanged(QColor)), this, SLOT(teamColorChanged(QColor)));
	selectionLayout->addWidget(_hueSlider, 1, 1);

	/** Add Button */
	QPushButton* addBtn = new QPushButton;
	QPixmap pixmap(":/imgs/add_icon_yellow.png");
	QIcon icon(pixmap);
	addBtn->setIcon(icon);
	addBtn->setIconSize(QSize(50, 50));
	addBtn->setObjectName("addButton");
	connect(addBtn, SIGNAL(released()), this, SLOT(addTeam()));
	selectionLayout->addWidget(addBtn, 1, 2);

	/** Start Quiz Button */
	QPushButton* startBtn = new QPushButton("Start Quiz");
	startBtn->setObjectName("menuButton");
	connect(startBtn, SIGNAL(released()), this, SLOT(teamSelected()));

	/** Quit Button */
	QPushButton* quitBtn = new QPushButton("Quit");
	quitBtn->setObjectName("menuButton");
	connect(quitBtn, SIGNAL(released()), this, SLOT(quit()));

	/** Add Widgets */
	mainlayout->addItem(selectionLayout, ++row, 0, 1, 3);
	mainlayout->addWidget(_teamTable, ++row, 0, 1, 3);
	mainlayout->addWidget(startBtn, ++row, 0, 1, 2);
	mainlayout->addWidget(quitBtn, row, 2, 1, 1);

	/** Set Focus on Line Edit */
	_teamNameLineEdit->setFocus();

	/** Set Layout */
	setLayout(mainlayout);
	setContentsMargins(20, 20, 20, 20);
}

void MusicQuiz::TeamSelector::teamSelected()
{
	/** Sanity Check */
	if ( _teamTable == nullptr ) {
		LOG_ERROR("Failed to start quiz. Team Table not created.");
		return;
	}

	/** Get Teams */
	std::vector<MusicQuiz::QuizTeam*> teams;
	for ( int i = 0; i < _teamTable->rowCount(); ++i ) {
		const QString teamName = _teamTable->item(i, 1)->text();
		const QColor teamColor = _teamTable->item(i, 1)->background().color();
		MusicQuiz::QuizTeam* teamEntry = new MusicQuiz::QuizTeam(teamName, teamColor);

		teams.push_back(teamEntry);
	}

	QString msg = QString::fromStdString("Are you sure you want to start the quiz?");
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Start Quiz?", msg,
		QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

	if ( resBtn == QMessageBox::Yes ) {
		emit teamSelectedSignal(teams);
	}
}

void MusicQuiz::TeamSelector::addTeam()
{
	/** Sanity Check */
	if ( _teamTable == nullptr || _teamNameLineEdit == nullptr || _hueSlider == nullptr ) {
		LOG_ERROR("Failed to add team. Team Selector not created correctly.");
		return;
	}

	QString teamName = _teamNameLineEdit->text();
	if ( teamName == "" ) {
		LOG_INFO("Can not add a team with a team name.");
		return;
	}

	/** Limit Number of teams */
	const int row = _teamTable->rowCount();
	if ( row >= 8 ) {
		LOG_INFO("Maximum number of teams reached.");
		return;
	}

	/** Check if team exisis */
	for ( int i = 0; i < row; ++i ) {
		if ( teamName == _teamTable->item(i, 1)->text() ) {
			LOG_INFO("Team name alredy exists.");
			return;
		}
	}

	/** Get Text color */
	QColor textColor = QColor(255 - _currentColor.red(), 255 - _currentColor.green(), 255 - _currentColor.blue());

	/** Insert Row */
	_teamTable->insertRow(row);

	/** Add Team Number */
	QTableWidgetItem* entry = new QTableWidgetItem;
	const QString teamNumber = " Team " + QString::number(row + 1);
	entry->setData(Qt::DisplayRole, teamNumber);
	entry->setTextAlignment(Qt::AlignCenter);
	entry->setForeground(QBrush(QColor(255, 255, 0)));
	_teamTable->setItem(row, 0, entry);

	/** Add Entry */
	entry = new QTableWidgetItem;
	entry->setData(Qt::DisplayRole, teamName);
	entry->setBackground(QBrush(_currentColor));
	entry->setTextAlignment(Qt::AlignCenter);
	entry->setForeground(QBrush(textColor));
	entry->setFlags(entry->flags() ^ Qt::ItemIsEditable); // make item read only
	_teamTable->setItem(row, 1, entry);

	/** Add Remove Team Button */
	QPushButton* removeBtn = new QPushButton;
	removeBtn->setObjectName("removeBtn");
	removeBtn->setProperty("teamName", teamName);
	connect(removeBtn, SIGNAL(released()), this, SLOT(removeTeam()));

	QHBoxLayout* removeBtnLayout = new QHBoxLayout;
	removeBtnLayout->addWidget(removeBtn, Qt::AlignCenter);

	QWidget* layoutWidget = new QWidget;
	layoutWidget->setLayout(removeBtnLayout);
	_teamTable->setCellWidget(row, 2, layoutWidget);

	/** Choose a random color for the team */
	std::random_device dev;
	std::mt19937 rng(dev());
	const int currentValue = _hueSlider->value();
	int newColorValue = currentValue;
	std::uniform_int_distribution<int> dist(_hueSlider->minimum(), _hueSlider->maximum());
	while ( std::fabs(currentValue - newColorValue) < (_hueSlider->maximum() - _hueSlider->minimum()) / 8 ) { // Ensure that the value is far enough away from previous color.
		newColorValue = dist(rng);
	}
	_hueSlider->setValue(newColorValue);

	/** Clear Line Edit */
	_teamNameLineEdit->setText("");

	/** Disable Line Edit and slider if maximum number of teams is reached */
	if ( row + 1 >= static_cast<int>(_maximumsNumberOfTeams) ) {
		_hueSlider->setEnabled(false);
		_hueSlider->setValue(_hueSlider->maximum());

		_teamNameLineEdit->setEnabled(false);
		teamColorChanged(QColor(150, 150, 150));
	}
}

void MusicQuiz::TeamSelector::setTeamNumbers()
{
	for ( int i = 0; i < _teamTable->rowCount(); ++i ) {
		const QString teamNumber = " Team " + QString::number(i + 1);
		_teamTable->item(i, 0)->setData(Qt::DisplayRole, teamNumber);
	}
}

void MusicQuiz::TeamSelector::removeTeam()
{
	/** Sanity Check */
	if ( _teamTable == nullptr || sender() == nullptr ) {
		LOG_ERROR("Failed to remove team. Team Selector was not created correctly.");
		return;
	}

	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if ( button == nullptr ) {
		LOG_ERROR("Failed to remove team. Sender was nullptr.");
		return;
	}

	/** Get Selected Row */
	int selectedRow = 0;
	for ( int i = 0; i < _teamTable->rowCount(); ++i ) {
		if ( button->property("teamName") == _teamTable->item(i, 1)->text() ) {
			selectedRow = i;
			break;
		}
	}

	/** Get Row Count */
	const int rows = _teamTable->rowCount();
	if ( selectedRow > rows ) {
		LOG_ERROR("Failed to remove team. Selected row is out of range.");
		return;
	}

	/** Remove Team */
	_teamTable->removeRow(selectedRow);

	/** Delete button */
	delete button;
	button = nullptr;

	/** Enable Line Edit and slider if count is below the maximum number of teams */
	if ( !_teamNameLineEdit->isEnabled() ) {
		_teamNameLineEdit->setEnabled(true);
	}

	if ( !_hueSlider->isEnabled() ) {
		_hueSlider->setEnabled(true);
		_hueSlider->setValue(_hueSlider->minimum());
	}
	setTeamNumbers();
}

void MusicQuiz::TeamSelector::teamColorChanged(QColor color)
{
	/** Set Color */
	_currentColor = color;

	/** Update Line Edit color */
	std::stringstream ss;
	ss << "background-color: rgb(" << color.red() << ", " << color.green() << ", " << color.blue() << ");"
		<< "color: rgb(" << 255 - color.red() << ", " << 255 - color.green() << ", " << 255 - color.blue() << ");";
	_teamNameLineEdit->setStyleSheet(QString::fromStdString(ss.str()));
}

void MusicQuiz::TeamSelector::quit()
{
	closeWindow();
}

void MusicQuiz::TeamSelector::closeEvent(QCloseEvent* event)
{
	if ( _quizClosed || closeWindow() ) {
		event->accept();
	} else {
		event->ignore();
	}
}

bool MusicQuiz::TeamSelector::closeWindow()
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

void MusicQuiz::TeamSelector::keyPressEvent(QKeyEvent* event)
{
	switch ( event->key() )
	{
	case Qt::Key_Escape:
		closeWindow();
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return:
		addTeam();
		break;
	default:
		QWidget::keyPressEvent(event);
		break;
	}
}