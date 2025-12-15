#include "QuizBoard.hpp"

#include <stdexcept>
#include <algorithm>

#include <QLabel>
#include <QScreen>
#include <QWindow>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QGuiApplication>
#include <QGraphicsBlurEffect>
#include <QPainter>

#include "common/Log.hpp"

#include "util/QuizSettings.hpp"
#include "gui_tools/widgets/QuizCategory.hpp"

#include "gui_tools/GuiUtil/QExtensions/QPushButtonExtender.hpp"

#include "LightDeviceConnectedWidget.hpp"
#include "lightcontrol/client/messages/SetColor.hpp"
#include "lightcontrol/client/messages/SetOn.hpp"
#include "lightcontrol/client/messages/SetEffect.hpp"


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
		_rowCategories.insert(_rowCategories.end(), rowCategories.begin(), rowCategories.end());
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

	/** Create countdown clock */
	if ( _settings.guessTimeLimit ) {
		const QRect screenRect = QGuiApplication::primaryScreen()->geometry();
		const int clockSize = screenRect.height() * 0.08;
		_countdownClock = new MusicQuiz::QExtensions::QCountDownClock(_settings.timeLimit, clockSize);
		_countdownClock->move(QPoint(screenRect.width() - (clockSize + screenRect.width() * 0.01), screenRect.height() * 0.02));
	}

	/** Initialize bingo if enabled */
	if ( _settings.bingoEnabled ) {
		const int cols = static_cast<int>( _categories.size() );
		const int rows = static_cast<int>( _categories[0]->getSize() );
		_cellOwner.assign(cols, std::vector<int>(rows, -1));

		/** Initialize bingo awarded trackers per team */
		_bingoRowAwarded.assign(_teams.size(), std::vector<bool>(rows, false));
		_bingoColAwarded.assign(_teams.size(), std::vector<bool>(cols, false));
		_bingoDiagAwarded.assign(_teams.size(), std::vector<bool>(2, false));
	}
}

void MusicQuiz::QuizBoard::lightClientConnectedCallback(LightControl::LightControlClient* client)
{
	client->sendMessage(LightControl::SetOn(false, 255));
	client->sendMessage(LightControl::SetEffect(LightControl::WledEffects::SOLID, 255, 255));
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
		categorylayout->setStretch(i, 1);
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
				connect(quizEntry, SIGNAL(blurQuiz()), this, SLOT(blurQuiz()));
				connect(quizEntry, SIGNAL(unBlurQuiz()), this, SLOT(unBlurQuiz()));
				connect(quizEntry, SIGNAL(startCountdown()), this, SLOT(startCountdown()));
				connect(quizEntry, SIGNAL(stopCountdown()), this, SLOT(stopCountdown()));
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
		if ( _lightClient != nullptr ) {
			LightDeviceConnectedWidget* connectedWidget = new LightDeviceConnectedWidget(_lightClient, this);
			connectedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			connectedWidget->setObjectName("QuizEntry_rowCategoryLabel");
			rowCategorylayout->addWidget(connectedWidget);
		}

		/** Add Row Categories */
		for ( size_t i = 0; i < _rowCategories.size(); ++i ) {
			QPushButton* rowCategoryBtn = new QPushButton(_rowCategories[i], this);
			rowCategoryBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			rowCategoryBtn->setObjectName("QuizEntry_rowCategoryLabel");
			rowCategorylayout->addWidget(rowCategoryBtn);
			_rowCategoryButtons.push_back(rowCategoryBtn);
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
	msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

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
	if ( _lightClient != nullptr ) {
		_lightClient->sendMessage(LightControl::SetEffect(LightControl::WledEffects::SOLID, 255, 255));
		_lightClient->sendMessage(LightControl::SetColor(
			static_cast<uint8_t>(buttonColor.red()),
			static_cast<uint8_t>(buttonColor.green()),
			static_cast<uint8_t>(buttonColor.blue())));
		_lightClient->sendMessage(LightControl::SetOn(true, 255));
	}

	/** Set Button Color */
	MusicQuiz::QuizEntry* entryButton = dynamic_cast<MusicQuiz::QuizEntry*>(sender());
	if ( entryButton != nullptr ) {
		entryButton->setColor(buttonColor);
	}

	/** Set Category button color */
	MusicQuiz::QuizCategory* categoryLabel = dynamic_cast<MusicQuiz::QuizCategory*>(sender());
	if ( _settings.guessTheCategory && categoryLabel != nullptr ) {
		categoryLabel->setCategoryColor(buttonColor);
		handleGameComplete();
		return;
	}

	/** Handle Bingo */
	handleBingo(entryButton, team);
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

		if ( _lightClient != nullptr ) {
			_lightClient->sendMessage(LightControl::SetEffect(LightControl::WledEffects::TWINKLE_CAT, 255, 128));
			_lightClient->sendMessage(LightControl::SetColor(255, 255, 255));
			_lightClient->sendMessage(LightControl::SetOn(true, 255));
		}

		emit gameComplete(winningTeams);
	} else if ( isGameComplete || _quizStopped ) {
		emit gameComplete({});
	}
}

void MusicQuiz::QuizBoard::handleBingo(MusicQuiz::QuizEntry* entry, MusicQuiz::QuizTeam* team)
{
	/** Sanity Check */
	if ( entry == nullptr || team == nullptr || _bingoPixmap.isNull() || !_settings.bingoEnabled ) {
		return;
	}

	/** Loop through the quiz and find the row and column that was answered */
	int foundColumn = -1;
	int foundRow = -1;
	for ( int column = 0; column < static_cast<int>(_categories.size()); ++column ) {
		for ( int row = 0; row < static_cast<int>(_categories[column]->getSize()); ++row ) {
			if ( ( *_categories[column] )[row] == entry ) {
				foundColumn = column;
				foundRow = row;
				break;
			}
		}

		if ( foundColumn != -1 ) {
			break;
		}
	}

	/** Check if column and row was found */
	if ( foundColumn == -1 && foundRow == -1 ) {
		return;
	}

	/** Get team index */
	int teamIdx = -1;
	for ( size_t t = 0; t < _teams.size(); ++t ) {
		if ( _teams[t] == team ) {
			teamIdx = static_cast<int>(t);
			break;
		}
	}

	if ( teamIdx == -1 ) {
		return;
	}

	/** Check for bingo */
	int newBingo = 0;
	_cellOwner[foundColumn][foundRow] = teamIdx;

	/** Check row bingo */
	bool rowBingo = true;
	for ( int column = 0; column < static_cast<int>(_categories.size()); ++column ) {
		if ( _cellOwner[column][foundRow] != teamIdx ) {
			rowBingo = false;
			break;
		}
	}

	if ( rowBingo && !_bingoRowAwarded[teamIdx][foundRow] ) {
		_bingoRowAwarded[teamIdx][foundRow] = true;
		++newBingo;
	}

	/** Check column bingo */
	bool columnBingo = true;
	for ( int row = 0; row < static_cast<int>(_categories[0]->getSize()); ++row ) {
		if ( _cellOwner[foundColumn][row] != teamIdx ) {
			columnBingo = false;
			break;
		}
	}

	if ( columnBingo && !_bingoColAwarded[teamIdx][foundColumn] ) {
		_bingoColAwarded[teamIdx][foundColumn] = true;
		++newBingo;
	}

	/** Check diagonals */
	if ( static_cast<int>( _categories.size() ) == static_cast<int>( _categories[0]->getSize() ) ) {
		bool diagMain = true;
		for ( int i = 0; i < static_cast<int>(_categories.size()); ++i ) {
			if ( _cellOwner[i][i] != teamIdx ) {
				diagMain = false;
				break;
			}
		}

		if ( diagMain && !_bingoDiagAwarded[teamIdx][0] ) {
			_bingoDiagAwarded[teamIdx][0] = true;
			++newBingo;
		}

		// anti-diagonal: col + row == n-1
		bool diagAnti = true;
		int n = static_cast<int>( _categories.size() );
		for ( int i = 0; i < n; ++i ) {
			if ( _cellOwner[i][n - 1 - i] != teamIdx ) {
				diagAnti = false;
				break;
			}
		}

		if ( diagAnti && !_bingoDiagAwarded[teamIdx][1] ) {
			_bingoDiagAwarded[teamIdx][1] = true;
			++newBingo;
		}
	}

	/** Show Bingo Image On screen for a few seconds then remove it with a timer */
	if ( newBingo > 0 ) {
		/** Create Image Label */
		QVBoxLayout* layout = new QVBoxLayout;
		QLabel* imageLabel = new QLabel(this);
		layout->addWidget(imageLabel);
		
		/** Set Atributes */
		imageLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
		imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		imageLabel->setAlignment(Qt::AlignCenter);

		/** Set Size and Position */
		const QRect screenRec = QGuiApplication::primaryScreen()->geometry();
		const int width = static_cast<int>( screenRec.width() * 0.7 );
		const int height = static_cast<int>( screenRec.height() * 0.7 );
		imageLabel->setMinimumSize(QSize(width, height));
		imageLabel->resize(QSize(width, height));
		imageLabel->move(QGuiApplication::primaryScreen()->geometry().center() - imageLabel->rect().center());

		/** Set Image and draw points text */
		QPixmap bingoOverlayPixmap = _bingoPixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

		/** Setup painter */
		QPainter painter(&bingoOverlayPixmap);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

		/** Setup font */
		QFont font = painter.font();
		font.setBold(true);
		int fontPointSize = std::max(12, static_cast<int>( bingoOverlayPixmap.height() * 0.04));
		font.setPointSize(fontPointSize);
		painter.setFont(font);

		QRect rect = bingoOverlayPixmap.rect();
		QRect textRect(rect.left(), rect.bottom() - (fontPointSize * 2) - 10, rect.width(), fontPointSize * 2 + 10);

		/** Add text */
		QPen yellowPen(Qt::yellow);
		yellowPen.setWidth(1);
		painter.setPen(yellowPen);
		painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, "+" + QString::number(static_cast<int>( _settings.bingoPoints * newBingo )));
		painter.end();

		/** Show image */
		imageLabel->setPixmap(bingoOverlayPixmap);
		imageLabel->show();

		/** Remove image after 5 seconds and clean up */
		QTimer::singleShot(5000, this, [imageLabel, this]() {
			if ( imageLabel != nullptr ) {
				imageLabel->hide();
				imageLabel->deleteLater();
			}
		});

		/** Add points */
		team->addPoints(_settings.bingoPoints * newBingo);
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

void MusicQuiz::QuizBoard::showEvent(QShowEvent*) 
{
	/** Sanity Check */
	if ( _rowCategoryButtons.empty() ) {
		return;
	}

	/** Resize Row Category Fonts */
	for ( size_t i = 0; i < _rowCategoryButtons.size(); ++i ) {
		int textWidth = _rowCategoryButtons[i]->fontMetrics().horizontalAdvance(_rowCategoryButtons[i]->text());
		size_t fontSize = 40;
		while ( textWidth > _rowCategoryButtons[i]->width() - 40 && fontSize > 10U ) {
			_rowCategoryButtons[i]->setStyleSheet("font-size: " + QString::number(fontSize) + "px;");
			textWidth = _rowCategoryButtons[i]->fontMetrics().horizontalAdvance(_rowCategories[i]);
			--fontSize;
		}

		const std::string stylesheetString = "font-size: " + std::to_string(fontSize) + "px;";
		_rowCategoryButtons[i]->setStyleSheet(QString::fromStdString(stylesheetString));
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

void MusicQuiz::QuizBoard::blurQuiz()
{
	QGraphicsBlurEffect* blur = new QGraphicsBlurEffect(this);
	blur->setBlurRadius(10);
	setGraphicsEffect(blur);
}

void MusicQuiz::QuizBoard::unBlurQuiz()
{
	setGraphicsEffect(nullptr);
}

void MusicQuiz::QuizBoard::startCountdown()
{
	if ( _countdownClock != nullptr ) {
		_countdownClock->start();
	}
}

void MusicQuiz::QuizBoard::stopCountdown()
{
	if ( _countdownClock != nullptr ) {
		_countdownClock->stop();
	}
}