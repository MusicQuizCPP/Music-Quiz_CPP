#pragma once

#include <QTabBar>


namespace MusicQuiz {
	namespace QExtensions {
		/**
		 * @brief \hack Tab bar specifically to get the tab on left side.
		 */
		class QTabBarExtender : public QTabBar {
		public:

			/**
			 * @brief Default constructor.
			 */
			QTabBarExtender() = default;

			/**
			 * @brief Default destructor.
			 */
			virtual ~QTabBarExtender() = default;

			/**
			 * @brief Deleted the copy and assignment constructor.
			 */
			QTabBarExtender(const QTabBarExtender&) = delete;
			QTabBarExtender& operator=(const QTabBarExtender&) = delete;

			QSize tabSizeHint(int index) const;

		protected:
			void paintEvent(QPaintEvent* /*event*/);
		};
	}
}
