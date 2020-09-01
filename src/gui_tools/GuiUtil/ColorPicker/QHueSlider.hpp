#pragma once

#include <QColor>
#include <QWidget>
#include <QObject>

#include "QGradientSlider.hpp"


namespace ColorPicker {
    /**
     * @brief A slider for selecting a hue value
     * @note Taken from https://gitlab.com/mattia.basaglia/Qt-Color-Widgets/-/tree/master/
     */
    class QHueSlider : public QGradientSlider {
        Q_OBJECT
            /** Saturation used in the rainbow gradient, as a [0-1] float  */
            Q_PROPERTY(qreal colorSaturation READ colorSaturation WRITE setColorSaturation NOTIFY colorSaturationChanged)

            /** Value used in the rainbow gradient, as a [0-1] float */
            Q_PROPERTY(qreal colorValue READ colorValue WRITE setColorValue NOTIFY colorValueChanged)

            /** Alpha used in the rainbow gradient, as a [0-1] float  */
            Q_PROPERTY(qreal colorAlpha READ colorAlpha WRITE setColorAlpha NOTIFY colorAlphaChanged)

            /** Color with corresponding color* components */
            Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

            /** Normalized Hue, as indicated from the slider */
            Q_PROPERTY(qreal colorHue READ colorHue WRITE setColorHue NOTIFY colorHueChanged)

    public:
        /**
         * @brief Constructor.
         *
         * @param[in] parent The parent widget.
         */
        explicit QHueSlider(QWidget* parent = nullptr);

        /**
         * @brief Constructor.
         *
         * @param[in] orientation The slider orientation.
         * @param[in] parent The parent widget.
         */
        explicit QHueSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

        /**
         * @brief Destructor
         */
        virtual ~QHueSlider();

        /**
         * @brief Gets the color.
         *
         * @return The color.
         */
        QColor color() const;

        /**
         * @brief Gets the hue color.
         *
         * @return The color in Hue.
         */
        qreal colorHue() const;

        /**
         * @brief Gets the color value.
         *
         * @return The color value.
         */
        qreal colorValue() const;

        /**
         * @brief Gets the alpha color.
         *
         * @return The alpha color.
         */
        qreal colorAlpha() const;

        /**
         * @brief Gets the color saturation.
         *
         * @return The color saturation.
         */
        qreal colorSaturation() const;

    public slots:
        void setColorAlpha(qreal alpha);
        void setColorValue(qreal value);
        void setColorHue(qreal colorHue);
        void setColorSaturation(qreal value);

        /**
         * @brief Sets the Hue Saturation and ColorValue,
         * @note Ignores the alpha value.
         *
         * @param[in] color The color.
         */
        void setColor(const QColor& color);

        /**
         * @brief Sets the Hue Saturation, ColorValue and Alpha
         *
         * @param[in] color The color.
         */
        void setFullColor(const QColor& color);

    signals:
        void colorChanged(QColor);
        void colorAlphaChanged(qreal v);
        void colorValueChanged(qreal v);
        void colorSaturationChanged(qreal v);
        void colorHueChanged(qreal colorHue);

    private:
        class Private;
        Private* const _privateMemeber;
    };


    class QHueSlider::Private {
    private:
        QHueSlider* _widget;

    public:
        qreal value = 1;
        qreal alpha = 1;
        qreal saturation = 1;

        Private(QHueSlider* widget) : _widget(widget)
        {
            widget->setRange(0, 359);
            connect(_widget, &QSlider::valueChanged, [this] {
                emit _widget->colorHueChanged(_widget->colorHue());
                emit _widget->colorChanged(_widget->color());
            });
            updateGradient();
        }

        void updateGradient()
        {
            static const int n_colors = 6;
            QGradientStops colors;
            colors.reserve(n_colors + 1);
            for ( int i = 0; i <= n_colors; ++i ) {
                colors.append(QGradientStop(i / static_cast<double>(n_colors), QColor::fromHsvF(i / static_cast<double>(n_colors), saturation, value)));
            }
            _widget->setColors(colors);
        }
    };
}
