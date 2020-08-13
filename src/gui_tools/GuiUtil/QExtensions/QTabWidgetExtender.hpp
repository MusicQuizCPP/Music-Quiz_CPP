#pragma once

#include <QTabWidget>


namespace MusicQuiz {
	namespace QExtensions {
		class QTabWidgetExtender : public QTabWidget {
		public:
			/**
			 * @brief Constructor.
			 *
			 * @param[in] parent The parent widget.
			 */
			explicit QTabWidgetExtender(QWidget* parent = nullptr);

			/**
			 * @brief Default destructor.
			 */
			virtual ~QTabWidgetExtender() = default;

			/**
			 * @brief Deleted the copy and assignment constructor.
			 */
			QTabWidgetExtender(const QTabWidgetExtender&) = delete;
			QTabWidgetExtender& operator=(const QTabWidgetExtender&) = delete;
		};
	}
}