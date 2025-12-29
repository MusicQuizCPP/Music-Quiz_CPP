#include "FontUtil.hpp"

#include <QFontMetrics>

QFont util::FontUtil::fittedFont(const QString & text, const QRect & rect, const QFont & baseFont)
{
	/** Initialize variables */
	QFont font = baseFont;
	int minFontSize = 1;
	int maxFontSize = 500;
	int best = minFontSize;
	QRect contentRect = rect;
	contentRect.adjust(8, 6, -8, -6);

	/** Binary search for best fitting font size */
	while ( minFontSize <= maxFontSize ) {
		const int mid = ( minFontSize + maxFontSize ) / 2;
		font.setPixelSize(mid);
		QFontMetrics fontMetrics(font);

		if ( fontMetrics.horizontalAdvance(text) <= contentRect.width() &&
			fontMetrics.height() <= contentRect.height() ) {
			best = mid;
			minFontSize = mid + 1;
		} else {
			maxFontSize = mid - 1;
		}
	}

	/** Set and return best fitting font */
	font.setPixelSize(best);
	return font;
}