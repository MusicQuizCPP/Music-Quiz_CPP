#include "QuizCategory.hpp"

#include <string>
#include <stdexcept>

#include <QLabel>
#include <QVBoxLayout>

#include "common/Log.hpp"

#include "gui_tools/widgets/QuizEntry.hpp"


MusicQuiz::QuizCategory::QuizCategory(QString name, const std::vector<MusicQuiz::QuizEntry*>& entries, QWidget* parent) :
	QWidget(parent), _name(name), _entries(entries)
{
	/** Sanity Check */
	if ( _name.isEmpty() ) {
		throw std::runtime_error("Cannot create category without a name.");
	}

	if ( _entries.empty() ) {
		throw std::runtime_error("Cannot create category with empty list of entries.");
	}

	/** Create Widget Layout */
	createLayout();
}

void MusicQuiz::QuizCategory::createLayout()
{
	/** Layout */
	QVBoxLayout* mainlayout = new QVBoxLayout;
	mainlayout->setContentsMargins(0, 0, 0, 0);
	mainlayout->setSpacing(10);

	/** Category Name */
	_categoryBtn = new MusicQuiz::QExtensions::QPushButtonExtender(this);
	_categoryBtn->setObjectName("QuizEntry_categoryLabel");
	_categoryBtn->setText(QString::fromLocal8Bit(_name.toStdString().c_str()));
	_categoryBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(_categoryBtn, SIGNAL(leftClicked()), this, SLOT(leftClickEvent()));
	connect(_categoryBtn, SIGNAL(rightClicked()), this, SLOT(rightClickEvent()));
	mainlayout->addWidget(_categoryBtn);

	/** Add Entries */
	for ( size_t i = 0; i < _entries.size(); ++i ) {
		mainlayout->addWidget(_entries[i]);
	}

	/** Set Layout */
	setLayout(mainlayout);
}

void MusicQuiz::QuizCategory::enableGuessTheCategory(const size_t points)
{
	_guessTheCategory = true;
	_points = points;

	if ( _guessTheCategory && _categoryBtn != nullptr ) {
		_categoryBtn->setText("?");
	}
}

size_t MusicQuiz::QuizCategory::getSize()
{
	return _entries.size();
}

void MusicQuiz::QuizCategory::leftClickEvent()
{
	if ( !_guessTheCategory ) {
		return;
	}

	switch ( _state ) {
	case CategoryState::IDLE:
		_state = CategoryState::GUESSED;
		_categoryBtn->setText(QString::fromLocal8Bit(_name.toStdString().c_str()));
		updateTextSize();
		emit guessed(_points);
		break;
	case CategoryState::GUESSED:
		break;
	default:
		throw std::runtime_error("Unknown Quiz Entry State Encountered.");
		break;
	}
}

void MusicQuiz::QuizCategory::rightClickEvent()
{
	if ( _categoryBtn == nullptr || !_guessTheCategory ) {
		return;
	}

	switch ( _state ) {
	case CategoryState::IDLE:
		break;
	case CategoryState::GUESSED:
		_categoryBtn->setText("?");
		setCategoryColor(QColor(0, 0, 255));
		_state = CategoryState::IDLE;
		break;
	default:
		throw std::runtime_error("Unknown Quiz Entry State Encountered.");
		break;
	}
}

void MusicQuiz::QuizCategory::setCategoryColor(const QColor& color)
{
	if ( _categoryBtn == nullptr || !_guessTheCategory ) {
		return;
	}

	_categoryBtn->setColor(color);
}

bool MusicQuiz::QuizCategory::hasCateogryBeenGuessed()
{
	if ( _categoryBtn == nullptr || !_guessTheCategory ) {
		return true;
	}

	if ( _state == CategoryState::GUESSED ) {
		return true;
	}

	return false;
}

void MusicQuiz::QuizCategory::updateTextSize()
{
	/** Text Size */
	int textWidth = _categoryBtn->fontMetrics().boundingRect(_name).width();
	size_t fontSize = 40;
	while ( textWidth > _categoryBtn->width() - 40 && fontSize > 10U ) {
		_categoryBtn->setStyleSheet("font-size: " + QString::number(fontSize) + "px;");
		textWidth = _categoryBtn->fontMetrics().boundingRect(_name).width();
		--fontSize;
	}

	const std::string stylesheetString = "font-size: " + std::to_string(fontSize) + "px;";
	_categoryBtn->setStyleSheet(QString::fromStdString(stylesheetString));
}

void MusicQuiz::QuizCategory::showEvent(QShowEvent* event)
{
	/** Accept the event */
	event->accept();

	/** Update Text Size if not in guess the category mode */
	if ( !_guessTheCategory ) {
		updateTextSize();
	}
}