#pragma once

#include "QPushButtonExtender.hpp"

#include <QColor>
#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QMouseEvent>


namespace MusicQuiz {
	namespace QExtensions {
		class NeonQPushButtonExtender : public QPushButtonExtender {
			Q_OBJECT

		public:
			/**
			 * @brief Constructor
			 *
			 * @param[in] parent The parent widget.
			 */
			explicit NeonQPushButtonExtender(QWidget* parent = nullptr);

			/**
			 * @brief Default Destructor
			 */
			virtual ~NeonQPushButtonExtender() = default;

		protected:
			/**
			 * @brief Overrides the paint event.
			 *
			 * @param[in] event The event.
			 */
			void paintEvent(QPaintEvent* event) override;
		};
	}
}