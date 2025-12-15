#pragma once

#include <QLabel>
#include <QWidget>
#include <QObject>
#include <QResizeEvent>


namespace MusicQuiz {
	namespace QExtensions {
		class QLabelExtender : public QLabel {
			Q_OBJECT

		public:
			/**
			 * @brief Constructor
			 *
			 * @param[in] parent The parent widget.
			 */
			explicit QLabelExtender(QWidget* parent = nullptr);

			/**
			 * @brief Default Destructor
			 */
			virtual ~QLabelExtender() = default;

			/**
			 * @brief Gets the rectangle of the label.
			 *
			 * @return The rectangle.
			 */
			QRect getRect();

			/**
			 * @brief Updates the layout of the label.
			 */
			void updateLayout();

		private slots:
			/**
			 * @brief Handles the resize event.
			 *
			 * @param[in] event The resize event.
			 */
			virtual void resizeEvent(QResizeEvent* event);
		};
	}
}