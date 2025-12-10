#include "QTabBarExtender.hpp"

#include <QStylePainter>
#include <QStyleOptionTab>


QSize MusicQuiz::QExtensions::QTabBarExtender::tabSizeHint(int index) const
{
	QSize s = QTabBar::tabSizeHint(index);
	s.transpose();
	s.setHeight(60);
	s.setWidth(150);
	return s;
}

void MusicQuiz::QExtensions::QTabBarExtender::paintEvent(QPaintEvent* /*event*/)
{
	QStylePainter painter(this);
	QStyleOptionTab opt;
	opt.rect.setHeight(60);
	opt.rect.setWidth(150);

	for ( int i = 0; i < count(); ++i ) {
		initStyleOption(&opt, i);
		painter.drawControl(QStyle::CE_TabBarTabShape, opt);
		painter.save();

		QSize s = opt.rect.size();
		s.transpose();

		QRect r(QPoint(), s);
		r.moveCenter(opt.rect.center());
		opt.rect = r;

		QPoint c = opt.rect.center();
		painter.translate(c);
		painter.rotate(90);
		painter.translate(-c - QPoint(0, 0));
		painter.drawControl(QStyle::CE_TabBarTabLabel, opt);
		painter.restore();
	}
}