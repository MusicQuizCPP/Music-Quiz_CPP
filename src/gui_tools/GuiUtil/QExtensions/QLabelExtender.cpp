#include "QLabelExtender.hpp"

#include <QStyle>



MusicQuiz::QExtensions::QLabelExtender::QLabelExtender(QWidget* parent) :
	QLabel(parent)
{
}

QRect MusicQuiz::QExtensions::QLabelExtender::getRect()
{
    QRect rect = contentsRect();
    int m = margin();
    rect.adjust(m, m, -m, -m);
    layoutDirection();
    const int align = QStyle::visualAlignment(layoutDirection(), QLabel::alignment());
    int i = indent();
    if ( i < 0 && frameWidth() ) {
        m = fontMetrics().width(QLatin1Char('x')) / 2 - m;
    }

    if ( m > 0 ) {
        if ( align & Qt::AlignLeft )
            rect.setLeft(rect.left() + m);
        if ( align & Qt::AlignRight )
            rect.setRight(rect.right() - m);
        if ( align & Qt::AlignTop )
            rect.setTop(rect.top() + m);
        if ( align & Qt::AlignBottom )
            rect.setBottom(rect.bottom() - m);
    }

    return rect;
}

void MusicQuiz::QExtensions::QLabelExtender::updateLayout()
{
    /** Get settings */
    QString text = this->text();
    QRect rect = getRect();
    QFont font = this->font();
    font.setPointSize(150);
    int size = font.pointSize();
    QFontMetrics fontMetrics(font);
    QRect boundingRect = fontMetrics.boundingRect(rect, Qt::TextWordWrap, text);

	/** Get step direction */
    int step = boundingRect.height() > rect.height() ? -5 : 5;

    /** Iterate until text fits into rectangle of the label */
    while ( true ) {
        font.setPointSize(size + step);
        QFontMetrics fontMetrics(font);
        boundingRect = fontMetrics.boundingRect(rect, Qt::TextWordWrap, text);

        /** Check if minimum font size have been reached */
        if ( size <= 1 ) {
            break;
        }

        /** Update size */
        if ( step < 0 ) {
            size += step;

            /** Check if the size fits within the layout. */
            if ( boundingRect.height() < rect.height() ) {
                break;
            }
        } else {
            /** Check if the size fits within the layout. */
            if ( boundingRect.height() > rect.height() ) {
                break;
            }

            size += step;
        }
    }

    /** Set new font size */
    font.setPointSize(size);
    setFont(font);
}

void MusicQuiz::QExtensions::QLabelExtender::resizeEvent(QResizeEvent* event)
{
    /** Accept event */
    QLabel::resizeEvent(event);

    /** Update layout */
    updateLayout();
}