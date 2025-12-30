#include "CategoryPushButtonExtender.hpp"

#include <sstream>

#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QStyleOptionButton>

#include "util/FontUtil.hpp"


MusicQuiz::QExtensions::CategoryPushButtonExtender::CategoryPushButtonExtender(QWidget* parent) :
    QPushButtonExtender(parent)
{}

void MusicQuiz::QExtensions::CategoryPushButtonExtender::paintEvent(QPaintEvent* event)
{
    /** Set up options and painter */
    QStyleOptionButton option;
    initStyleOption(&option);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    /** Draw base button */
    style()->drawControl(QStyle::CE_PushButtonBevel, &option, &p, this);
    style()->drawControl(QStyle::CE_PushButtonLabel, &option, &p, this);
    QRect r = rect();

    /** Top Line */
    QLinearGradient topGlow(r.topLeft(), r.topRight());
    topGlow.setColorAt(0.0, QColor(14, 81, 189));
    topGlow.setColorAt(0.5, QColor(13, 73, 171));
    topGlow.setColorAt(1.0, QColor(14, 81, 189));

    QPen topPen(topGlow, 5);
    topPen.setCapStyle(Qt::FlatCap);
    p.setPen(topPen);
    p.drawLine(QPoint(r.left(), r.top()), QPoint(r.right(), r.top()));

    /** Left Side */
    QLinearGradient leftGlow(r.topLeft(), r.bottomLeft());
    leftGlow.setColorAt(0.0, QColor(14, 81, 189));
    leftGlow.setColorAt(1.0, QColor(141, 223, 245));

    QPen leftPen(leftGlow, 7);
    leftPen.setCapStyle(Qt::FlatCap);
    p.setPen(leftPen);
    p.drawLine(QPoint(r.left(), r.top()), QPoint(r.left(), r.bottom()));

    /** Right Side */
    QLinearGradient rightGlow(r.topRight(), r.bottomRight());
    rightGlow.setColorAt(0.0, QColor(14, 81, 189));
    rightGlow.setColorAt(1.0, QColor(141, 223, 245));

    QPen rightPen(rightGlow, 7);
    rightPen.setCapStyle(Qt::FlatCap);
    p.setPen(rightPen);
    p.drawLine(QPoint(r.right(), r.top()), QPoint(r.right(), r.bottom()));

    /** Bottom */
    QLinearGradient bottomGlow(r.bottomLeft(), r.bottomRight());
    bottomGlow.setColorAt(0.0, QColor(141, 223, 245));
    bottomGlow.setColorAt(0.45, QColor(231, 252, 249));
    bottomGlow.setColorAt(0.5, QColor(255, 255, 255));
    bottomGlow.setColorAt(0.55, QColor(231, 252, 249));
    bottomGlow.setColorAt(1.0, QColor(141, 223, 245));

    QPen bottomPen(bottomGlow, 12);
    bottomPen.setCapStyle(Qt::FlatCap);
    p.setPen(bottomPen);
    p.drawLine(QPoint(r.left(), r.bottom()), QPoint(r.right(), r.bottom()));

    /** Get Text Rect */
    const QRect textRect = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);

    /** Create Fitted Font */
    QFont baseFont = font();
    QFont fitted = util::FontUtil::fittedFont(text(), textRect, baseFont);
    QFontMetrics fontMetrics(fitted);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setFont(fitted);

    /** Draw Text Shadow */
    QPoint shadowOffset(4, 4);
    QColor shadowColor(0, 0, 0, 150);

    p.setPen(shadowColor);
    p.drawText(textRect.translated(shadowOffset), Qt::AlignCenter, text());

    /** Draw Text */
    const QPointF start(textRect.center().x(), textRect.top());
    const QPointF end(textRect.center().x(), textRect.bottom());
    QLinearGradient gradient(start, end);
    gradient.setColorAt(0.0, QColor(255, 255, 255));
    gradient.setColorAt(1.0, QColor(210, 210, 210));

    p.setPen(QPen(QBrush(gradient), 0));
    p.drawText(textRect, Qt::AlignCenter, text());
}