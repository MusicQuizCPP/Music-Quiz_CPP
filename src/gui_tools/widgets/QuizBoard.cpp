#include "QuizBoard.hpp"

#include <stdexcept>
#include <algorithm>

#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QWindow>
#include <QScreen>

#include "common/Log.hpp"

#include "util/QuizSettings.hpp"
#include "gui_tools/widgets/QuizTeam.hpp"
#include "gui_tools/widgets/QuizEntry.hpp"
#include "gui_tools/widgets/QuizCategory.hpp"

#include "gui_tools/GuiUtil/QExtensions/QPushButtonExtender.hpp"

#include "LightDeviceConnectedWidget.hpp"

#include "lightcontrol/client/messages/LightModeMessage.hpp"
#include "lightcontrol/client/messages/OnBoardLEDStrength.hpp"


MusicQuiz::QuizBoard::QuizBoard(const std::vector<MusicQuiz::QuizCategory*>& categories, const std::vector<QString>& rowCategories,
	const std::vector<MusicQuiz::QuizTeam*>& teams, const MusicQuiz::QuizSettings& settings, bool preview, QWidget* parent) :
	QDialog(parent), _settings(settings), _teams(teams), _categories(categories)
{
	/** Set Object Name */
	setObjectName("QuizBoard");

	/** Set Window Flags */
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	if ( !preview ) {
		setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);

		/** Set Fullscreen */
		showFullScreen();
	}

	/** Sanity Check */
	if ( _categories.empty() ) {
		throw std::runtime_error("Cannot create quiz board without any categories.");
	}

	/** Create Light Controller */
	_lightClient = std::make_shared<LightControl::LightControlClient>(_settings.deviceIP, 80);
	_lightClient->addConnectedCallback(&MusicQuiz::QuizBoard::lightClientConnectedCallback);
	_lightClient->start();

	/** Set Row Categories if they match the number of entries in the categories */
	bool sameNumberOfEntries = true;
	for ( size_t i = 0; i < _categories.size(); ++i ) {
		if ( rowCategories.size() != _categories[i]->getSize() ) {
			sameNumberOfEntries = false;
			break;
		}
	}

	if ( sameNumberOfEntries ) {
		_rowCategories = rowCategories;
	}

	/** Create Widget Layout */
	createLayout();

	/** Install event filter in all child widgets */
	QList<QWidget*> widgets = findChildren<QWidget*>();
	for ( QWidget* widget : widgets ) {
		if ( widget != nullptr ) {
			widget->installEventFilter(this);
		}
	}
}

void MusicQuiz::QuizBoard::lightClientConnectedCallback(LightControl::LightControlClient* client)
{
	client->sendMessage(LightControl::OnBoardLEDStrength(0));
}

void MusicQuiz::QuizBoard::createLayout()
{
	/** Layout */
	QGridLayout* mainlayout = new QGridLayout;
	QHBoxLayout* categorylayout = new QHBoxLayout;
	QHBoxLayout* teamsLayout = new QHBoxLayout;
	mainlayout->setHorizontalSpacing(0);
	mainlayout->setVerticalSpacing(15);
	teamsLayout->setSpacing(10);
	categorylayout->setSpacing(10);

	/** Categories */
	size_t maxNumberOfEntries = 0;
	for ( size_t i = 0; i < _categories.size(); ++i ) {
		categorylayout->addWidget(_categories[i]);
		if ( _settings.guessTheCategory ) {
			connect(_categories[i], SIGNAL(guessed(size_t)), this, SLOT(handleAnswer(size_t)));
		}

		/** Connect Buttons */
		const size_t categorySize = _categories[i]->getSize();
		for ( size_t j = 0; j < categorySize; ++j ) {
			MusicQuiz::QuizEntry* quizEntry = (*_categories[i])[j];
			if ( quizEntry != nullptr ) {
				connect(quizEntry, SIGNAL(answered(size_t)), this, SLOT(handleAnswer(size_t)));
				connect(quizEntry, SIGNAL(played()), this, SLOT(handleGameComplete()));
			}
		}

		/** Get Maximum Number of Entries */
		if ( categorySize > maxNumberOfEntries ) {
			maxNumberOfEntries = categorySize;
		}
	}

	/** Row Categories */
	if ( !_rowCategories.empty() ) {
		QVBoxLayout* rowCategorylayout = new QVBoxLayout;
		rowCategorylayout->setSpacing(10);

		/** Add Light Device Status Box */
		LightDeviceConnectedWidget* connectedWidget = new LightDeviceConnectedWidget(_lightClient, this);
		connectedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		connectedWidget->setObjectName("QuizEntry_rowCategoryLabel");
		rowCategorylayout->addWidget(connectedWidget);

		/** Add Row Categories */
		for ( size_t i = 0; i < _rowCategories.size(); ++i ) {
			QPushButton* rowCategoryBtn = new QPushButton(_rowCategories[i], this);
			rowCategoryBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			rowCategoryBtn->setObjectName("QuizEntry_rowCategoryLabel");
			rowCategorylayout->addWidget(rowCategoryBtn);
		}

		/** Add layouts to main layout */
		QGridLayout* tmpLayout = new QGridLayout;
		tmpLayout->setSpacing(10);
		tmpLayout->addItem(rowCategorylayout, 0, 0);
		tmpLayout->addItem(categorylayout, 0, 1);
		tmpLayout->setColumnStretch(0, 1);
		tmpLayout->setColumnStretch(1, static_cast<int>(_categories.size() * 2));
		mainlayout->addItem(tmpLayout, 0, 0);
	} else {
		mainlayout->addItem(categorylayout, 0, 0);
	}

	/** Teams */
	for ( size_t i = 0; i < _teams.size(); ++i ) {
		_teams[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		teamsLayout->addWidget(_teams[i]);
	}
	mainlayout->addItem(teamsLayout, 1, 0);

	/** Set Row Stretch */
	if ( !_teams.empty() ) {
		mainlayout->setRowStretch(0, static_cast<int>(static_cast<double>(maxNumberOfEntries + 1) * 1.5));
		mainlayout->setRowStretch(1, 1);
	}

	/** Set Layout */
	setLayout(mainlayout);
}

void MusicQuiz::QuizBoard::handleAnswer(const size_t points)
{
	/** Sanity Check */
	if ( sender() == nullptr ) {
		return;
	}

	if ( _teams.empty() ) {
		return;
	}

	/** Select which team guessed the entry / category */
	QMessageBox msgBox(QMessageBox::Question, "Select Team", "Select Team", QMessageBox::NoButton, nullptr, Qt::WindowStaysOnTopHint);
	msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);

	/** Add Buttons for Each Team */
	std::vector< QAbstractButton* > teamButtons;
	for ( size_t i = 0; i < _teams.size(); ++i ) {
		teamButtons.push_back(msgBox.addButton(_teams[i]->getName(), QMessageBox::YesRole));
	}
	msgBox.addButton("No One", QMessageBox::YesRole);

	/** Move Box to the bottom of the screen */
	QSize size = msgBox.sizeHint();
	QRect screenRect = this->window()->windowHandle()->screen()->geometry();
	msgBox.move(QPoint(screenRect.width() / 2 - size.width() / 2, screenRect.height() - (size.height() * 2)));

	/** Box Message Box */
	msgBox.exec();

	/** Get Selected Team */
	MusicQuiz::QuizTeam* team = nullptr;
	for ( size_t i = 0; i < teamButtons.size(); ++i ) {
		if ( msgBox.clickedButton() == teamButtons[i] ) {
			team = _teams[i];
		}
	}

	/** Get Color & Add Points */
	QColor buttonColor(0, 0, 255);
	if ( team != nullptr ) {
		/** Add Points to the team */
		team->addPoints(points);

		/** Set button color */
		if ( _settings.hiddenTeamScore == false ) {
			buttonColor = team->getColor();
		}
	} else {
		/** Not guessed set button color to grey */
		if ( _settings.hiddenTeamScore == false ) {
			buttonColor = QColor(128, 128, 128);
		}
	}

	/** Set color on light device */
	_lightClient->sendMessage(LightControl::LightModeMessage( LightControl::LightMode::ON, 1.f,
																static_cast<uint8_t>(buttonColor.red()),
																static_cast<uint8_t>(buttonColor.green()),
																static_cast<uint8_t>(buttonColor.blue())));

	/** Set Button Color */
	MusicQuiz::QuizEntry* entryButton = dynamic_cast<MusicQuiz::QuizEntry*>(sender());
	if ( entryButton != nullptr ) {
		entryButton->setColor(buttonColor);
		return;
	}

	MusicQuiz::QuizCategory* categoryLabel = dynamic_cast<MusicQuiz::QuizCategory*>(sender());
	if ( _settings.guessTheCategory && categoryLabel != nullptr ) {
		categoryLabel->setCategoryColor(buttonColor);
		handleGameComplete();
		return;
	}
}

void MusicQuiz::QuizBoard::handleGameComplete()
{
	/** Check if game has ended */
	bool isGameComplete = true;
	for ( size_t i = 0; i < _categories.size(); ++i ) {
		if ( _settings.guessTheCategory ) {
			if ( !_categories[i]->hasCateogryBeenGuessed() ) {
				isGameComplete = false;
				break;
			}
		}

		for ( size_t j = 0; j < _categories[i]->getSize(); ++j ) {
			if ( (*_categories[i])[j]->getEntryState() != QuizEntry::EntryState::PLAYED ) {
				isGameComplete = false;
				break;
			}

		}
	}

	if ( (isGameComplete || _quizStopped) && !_teams.empty() ) {
		/** Find Winner */
		const size_t highScore = (*std::max_element(_teams.begin(), _teams.end(), [](const MusicQuiz::QuizTeam* a, const MusicQuiz::QuizTeam* b) {return a->getScore() < b->getScore(); }))->getScore();
		std::vector<MusicQuiz::QuizTeam*> winningTeams;
		for ( size_t i = 0; i < _teams.size(); ++i ) {
			if ( _teams[i]->getScore() == highScore ) {
				winningTeams.push_back(_teams[i]);
			}
		}

		emit gameComplete(winningTeams);
	} else if ( isGameComplete || _quizStopped ) {
		emit gameComplete({});
	}
}

void MusicQuiz::QuizBoard::setQuizName(const QString& name)
{
	_name = name;
}

QString MusicQuiz::QuizBoard::getQuizName()
{
	return _name;
}

void MusicQuiz::QuizBoard::closeEvent(QCloseEvent* event)
{
	if ( _quizClosed || closeWindow() ) {
		event->accept();
	} else {
		event->ignore();
	}
}

bool MusicQuiz::QuizBoard::closeWindow()
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

void MusicQuiz::QuizBoard::keyPressEvent(QKeyEvent* event)
{
	switch ( event->key() ) {
	case Qt::Key_Escape:
		closeWindow();
		break;
	case Qt::Key_Q: // Stop Quiz Before it is complete
		_quizStopped = true;
		handleGameComplete();
		break;
	default:
		QWidget::keyPressEvent(event);
		break;
	}
}

bool MusicQuiz::QuizBoard::eventFilter(QObject* target, QEvent* event)
{
	if ( event->type() == QEvent::KeyPress ) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if ( keyEvent->key() == Qt::Key_Escape ) {
			closeWindow();
			return true;
		}
	}

	return QDialog::eventFilter(target, event);
}