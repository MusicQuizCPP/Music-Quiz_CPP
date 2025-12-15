#pragma once

#include <string>

#include <QFrame>


class QLabel;
class QSlider;
class QLineEdit;

namespace gui_tools {
	namespace GuiUtil {
		/**
		 * @brief Widget to display slider settings in the common format.
		 *        | Title  |   value & unit |
		 *        | :-:    |            --: |
		 *        | --- ---(slider)--- --- ||
		 */
		class QSliderWidget : public QFrame
		{
			Q_OBJECT
		public:
			/**
			 * @brief Constructor.
			 *
			 * @param[in] title  The text to display on the label.
			 * @param[in] min    The minimum value to display.
			 * @param[in] max    The maximum value to display.
			 * @param[in] steps  The number of steps to divide the slider into.
			 * @param[in] prefix The prefix to the displayed value.
			 * @param[in] suffix The suffix to the displayed value.
			 * @param[in] prec   The precision to display the value with.
			 * @param[in] parent The parent widget.
			 */
			explicit QSliderWidget(const QString& title, double min, double max, int steps, const QString& prefix = "", const QString& suffix = "", int prec = 0, QWidget* parent = nullptr);

			/**
			 * @brief Default destructor.
			 */
			virtual ~QSliderWidget() = default;

			/**
			 * @brief Deleted the copy constructor.
			 */
			QSliderWidget(const QSliderWidget&) = delete;

			/**
			 * @brief Deleted the assignment constructor.
			 */
			QSliderWidget& operator=(const QSliderWidget&) = delete;

			/**
			 * @brief Get the value of the slider [min, max].
			 *
			 * @return The value.
			 */
			double getValue() const;

			/**
			 * @brief Set the value of the slider [min, max].
			 *
			 * @param[in] value The value to set it to.
			 */
			void setValue(double value);

			/**
			 * @brief Get the slider in use.
			 *
			 * @return The slider.
			 */
			QSlider* getSlider();

			/**
			 * @brief Sets the object name for the widget and all sub widgets.
			 *
			 * @param[in] objectName The object name to set.
			 */
			void setObjectNameOfWidgets(const QString& objectName);

		private slots:
			/**
			 * @brief Updates the displayed value in the line edit.
			 */
			void updateValueDisplay(int value);

		signals:
			/**
			 * @brief Indicates that the value of the slider has been changed.
			 */
			void valueChanged(int);

		protected:
			/**
			 * @brief Make the widget layout.
			 */
			void makeWidgetLayout();

			/** The title of the setting. */
			const QString _title = "";

			/** The minimum value of the slider. */
			double _min = 0.0;

			/** The maximum value of the slider. */
			double _max = 0.0;

			/** The number of steps of the slider between min and max, both included. */
			int _steps = 0;

			/** The prefix string to be displayed before the values. */
			QString _prefix = "";

			/** The suffx string to be displayed after the values. */
			QString _suffix = "";

			/** The precision to display the values with. */
			int _precision = 0;

			/** The label displaying the title. */
			QLabel* _widgetLabel = nullptr;

			/** The slider of the widget. */
			QSlider* _widgetSlider = nullptr;

			/** The line edit displaying the value and unit. */
			QLineEdit* _widgetLineEdit = nullptr;
		};
	}
}