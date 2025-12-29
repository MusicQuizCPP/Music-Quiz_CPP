#include "QPushButtonExtender.hpp"

#include <sstream>

#include <QFontDatabase>


MusicQuiz::QExtensions::QPushButtonExtender::QPushButtonExtender(QWidget* parent) :
	QPushButton(parent)
{
    /** Set font */
    const int fontIndex = QFontDatabase::addApplicationFont(":/fonts/BebasNeue-Regular.ttf");
    const QString fontFamily = QFontDatabase::applicationFontFamilies(fontIndex).at(0);

    QFont font(fontFamily);
    font.setCapitalization(QFont::AllUppercase);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
    setFont(font);
}

void MusicQuiz::QExtensions::QPushButtonExtender::mouseReleaseEvent(QMouseEvent* event)
{
	if ( event->button() == Qt::LeftButton ) {
		emit leftClicked();
	} else if ( event->button() == Qt::RightButton ) {
		emit rightClicked();
	}
}

void MusicQuiz::QExtensions::QPushButtonExtender::setColor(const QColor& color)
{
	std::stringstream ss;
	ss << "background-color	: rgb(" << color.red() << ", " << color.green() << ", " << color.blue() << ");";
	setStyleSheet(QString::fromStdString(ss.str()));
}