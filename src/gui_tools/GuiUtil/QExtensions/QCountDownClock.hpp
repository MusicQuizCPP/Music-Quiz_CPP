#pragma once

#include <QTimer>
#include <QWidget>
#include <QPaintEvent>


namespace MusicQuiz {
	namespace QExtensions {
		/**
		 * @brief A countdown clock.
		 */
		class QCountDownClock : public QWidget {
			Q_OBJECT

		public:
			/**
			 * @brief Constructor.
			 * 
			 * @param[in] countdown   The countdown time in msec.
			 * @param[in] widgetSize  The size of the clock widget.
			 * @param[in] parent      The parent widget.
			 */
			explicit QCountDownClock(const int countdown, const int widgetSize = 100, QWidget* parent = nullptr);

			/**
			 * @brief Default destructor.
			 */
			virtual ~QCountDownClock() = default;

			/**
			 * @brief Deleted the copy and assignment constructor.
			 */
			QCountDownClock(const QCountDownClock&) = delete;
			QCountDownClock& operator=(const QCountDownClock&) = delete;

			/**
			 * @brief Starts the countdown.
			 */
			void start();

			/**
			 * @brief Stops the countdown.
			 */
			void stop();

		protected:
			/**
			 * @brief Paint event.
			 * 
			 * @param[in] event The paint event.
			 */
			void paintEvent(QPaintEvent* event) override;

		private slots:
			/**
			 * @brief Updates the clock.
			 */
			void updateClock();

		private:
			/** Timer */
			QTimer* _updateTimer = nullptr;

			/** The countdown in msec */
			int _countdown;

			/** The elapsed time in msec */
			int _elapsedTime = 0;
		};
	}
}
