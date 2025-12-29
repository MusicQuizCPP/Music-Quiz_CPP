#pragma once

#include <QFont>
#include <QRect>
#include <QString>

namespace util {

	/**
	 * @brief Utility functions for manipulation of fonts.
	 */
	class FontUtil {
	public:
		/**
		 * @brief Deleted default constructor to prevent creation of object.
		 */
		FontUtil() = delete;

		/**
		 * @brief Deleted destructor to prevent creation of object.
		 */
		~FontUtil() = delete;

		/**
		 * @brief Delete copy constructor.
		 */
		FontUtil(const FontUtil&) = delete;

		/**
		 * @brief Delete move constructor.
		 */
		FontUtil(const FontUtil&&) = delete;

		/**
		 * @brief Delete copy assignment operator.
		 */
		FontUtil& operator=(const FontUtil&) = delete;

		/**
		 * @brief Delete move assignment operator.
		 */
		FontUtil& operator=(const FontUtil&&) = delete;

		/**
		 * @brief Updates the font to fit inside the given rect.
		 *
		 * @param[in] text     The text string.
		 * @param[in] rect     The rect to fit the font into.
		 * @param[in] baseFont The base font to update.
		 *
		 * @return The updated font that fits inside the rect.
		 */
		static QFont fittedFont(const QString& text, const QRect& rect, const QFont& baseFont);
	};
}