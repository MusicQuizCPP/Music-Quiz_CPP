#pragma once

#include <QTabBar>


namespace MusicQuiz {
	namespace QExtensions {
		/**
		 * @brief \hack Tab bar specifically to get the tab on left side.
		 */
		class QTabBarExtender : public QTabBar {
			Q_OBJECT

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
			/**
			 * @brief Paint event.
			 *
			 * @param[in] event The paint event.
			 */
			void paintEvent(QPaintEvent* event) override;
		};
	}
}
