#pragma once

#include <QColor>
#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QMouseEvent>


namespace MusicQuiz {
	namespace QExtensions {
		class QPushButtonExtender : public QPushButton {
			Q_OBJECT

		public:
			/**
			 * @brief Constructor
			 *
			 * @param[in] parent The parent widget.
			 */
			explicit QPushButtonExtender(QWidget* parent = nullptr);

			/**
			 * @brief Default Destructor
			 */
			virtual ~QPushButtonExtender() = default;

			/**
			 * @brief Sets the color of the button.
			 *
			 * @param[in] color The color.
			 */
			void setColor(const QColor& color);

		private slots:
			/**
			 * @brief Override the mouse release event.
			 *
			 * @param[in] event The event.
			 */
			void mouseReleaseEvent(QMouseEvent* e);

		signals:
			void leftClicked();
			void rightClicked();

		public slots:
		};
	}
}