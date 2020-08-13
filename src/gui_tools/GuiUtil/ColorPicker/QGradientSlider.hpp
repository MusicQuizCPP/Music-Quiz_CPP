#pragma once

#include <QColor>
#include <QSlider>
#include <QBrush>
#include <QWidget>
#include <QObject>
#include <QtGlobal>
#include <QGradient>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QGradientStops>
#include <QLinearGradient>


namespace ColorPicker {
	/**
	 * @brief A slider that moves on top of a gradient
	 * @note Taken from https://gitlab.com/mattia.basaglia/Qt-Color-Widgets/-/tree/master/
	 */
	class QGradientSlider : public QSlider {
		Q_OBJECT
			Q_PROPERTY(QLinearGradient gradient READ gradient WRITE setGradient)
			Q_PROPERTY(QColor lastColor READ lastColor WRITE setLastColor STORED false)
			Q_PROPERTY(QGradientStops colors READ colors WRITE setColors DESIGNABLE false)
			Q_PROPERTY(QColor firstColor READ firstColor WRITE setFirstColor STORED false)
			Q_PROPERTY(QBrush background READ background WRITE setBackground NOTIFY backgroundChanged)

	public:
		/**
		 * @brief Constructor.
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit QGradientSlider(QWidget* parent = nullptr);

		/**
		 * @brief Constructor.
		 *
		 * @param[in] orientation The slider orientation.
		 * @param[in] parent The parent widget.
		 */
		explicit QGradientSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

		/**
		 * @brief Destructor
		 */
		~QGradientSlider();

		/**
		 * @brief Gets the background.
		 * @note it's visible for transparent gradient stops.
		 *
		 * @return The background.
		 */
		QBrush background() const;

		/**
		 * @brief Sets the background.
		 * @note it's visible for transparent gradient stops.
		 *
		 * @param[in] bg The background.
		 */
		void setBackground(const QBrush& bg);

		/**
		 * @brief Gets the colors that make up the gradient.
		 *
		 * @return The color.
		 */
		QGradientStops colors() const;

		/**
		 * @brief Sets the colors that make up the gradient.
		 *
		 * @param[in] colors The color.
		 */
		void setColors(const QGradientStops& colors);

		/**
		 * @brief Sets the color that make up the gradient.
		 * @note Overload: create an evenly distributed gradient of the given colors.
		 *
		 * @param[in] colors The color.
		 */
		void setColors(const QVector<QColor>& colors);

		/**
		 * @brief Gets the first color.
		 *
		 * @returns The first color.
		 */
		QColor firstColor() const;

		/**
		 * @brief Set the first color of the gradient
		 * @note If the gradient is currently empty it will create a stop with the given color.
		 *
		 * @param[in] color The first color.
		 */
		void setFirstColor(const QColor& color);

		/**
		 * @brief Gets the last color.
		 *
		 * @returns The last color.
		 */
		QColor lastColor() const;

		/**
		 * @brief Set the last color of the gradient.
		 * @note If the gradient is has less than two colors, it will create a stop with the given color.
		 *
		 * @param[in] color The last color.
		 */
		void setLastColor(const QColor& color);

		/**
		 * @brief Gets the gradient.
		 *
		 * @return The gradient.
		 */
		QLinearGradient gradient() const;

		/**
		 * @brief Sets the gradient.
		 *
		 * @param[in] gradient The gradient.
		 */
		void setGradient(const QLinearGradient& gradient);

	Q_SIGNALS:
		void backgroundChanged(const QBrush&);

	protected:
		/** Events */
		void paintEvent(QPaintEvent* ev) override;
		void mousePressEvent(QMouseEvent* ev) override;
		void mouseMoveEvent(QMouseEvent* ev) override;
		void mouseReleaseEvent(QMouseEvent* ev) override;

	private:
		class Private;
		Private* const _privateMember;
	};


	class ColorPicker::QGradientSlider::Private {
	public:
		QBrush background;
		QLinearGradient gradient;

		Private() : background(Qt::darkGray, Qt::DiagCrossPattern)
		{
			gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
			gradient.setSpread(QGradient::RepeatSpread);
		}

		void mouse_event(QMouseEvent* mouseEvent, ColorPicker::QGradientSlider* owner)
		{
			qreal pos = (owner->geometry().width() > 5) ? static_cast<qreal>(mouseEvent->pos().x() - 2.5) / (owner->geometry().width() - 5) : 0;
			pos = qMax(qMin(pos, 1.0), 0.0);
			owner->setSliderPosition(qRound(owner->minimum() + pos * (owner->maximum() - owner->minimum())));
		}

	};
}