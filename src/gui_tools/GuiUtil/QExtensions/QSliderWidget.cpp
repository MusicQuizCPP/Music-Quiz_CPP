#include "QSliderWidget.hpp"

#include <cmath>
#include <stdexcept>

#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QGridLayout>


gui_tools::GuiUtil::QSliderWidget::QSliderWidget(const QString& title, double min, double max, int steps, const QString& prefix, const QString& suffix, int prec, QWidget* parent) :
	QFrame(parent), _title(title), _min(min), _max(max), _steps(steps), _prefix(prefix), _suffix(suffix), _precision(prec)
{
	/** Sanity Check */
	if ( _steps < 2 ) {
		throw std::runtime_error("Can not have less than two steps in the slider.");
	}

	if ( _max <= _min ) {
		throw std::runtime_error("The minimum value cannot be less than or equal to the maximum value on the slider.");
	}

	/** Create Layout */
	makeWidgetLayout();
}

void gui_tools::GuiUtil::QSliderWidget::makeWidgetLayout()
{
	/** Layout */
	QGridLayout* sliderLayout = new QGridLayout;
	sliderLayout->setVerticalSpacing(0);
	sliderLayout->setHorizontalSpacing(0);
	sliderLayout->setContentsMargins(0, 0, 0, 0);

	/** Label */
	_widgetLabel = new QLabel(_title);
	_widgetLabel->setObjectName("quizCreatorLabel");
	sliderLayout->addWidget(_widgetLabel, 0, 0, Qt::AlignLeft);

	/** LineEdit */
	_widgetLineEdit = new QLineEdit(_prefix + QString::number(_min, 'f', _precision) + _suffix);
	_widgetLineEdit->setObjectName("quizCreatorSliderLineEdit");
	_widgetLineEdit->setAlignment(Qt::AlignRight);
	_widgetLineEdit->setReadOnly(true);
	sliderLayout->addWidget(_widgetLineEdit, 0, 1, Qt::AlignRight);

	/** Slider */
	_widgetSlider = new QSlider(Qt::Horizontal, this);
	_widgetSlider->setRange(0, _steps - 1);
	_widgetSlider->setSingleStep(1);
	_widgetSlider->setPageStep(1);
	_widgetSlider->setValue(0);
	_widgetSlider->setObjectName("quizCreatorSlider");
	QObject::connect(_widgetSlider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
	QObject::connect(_widgetSlider, SIGNAL(valueChanged(int)), this, SLOT(updateValueDisplay(int)));
	sliderLayout->addWidget(_widgetSlider, 1, 0, 1, 2);

	/** Set Layout */
	setLayout(sliderLayout);
}

double gui_tools::GuiUtil::QSliderWidget::getValue() const
{
	/** Sanity Check */
	if ( _widgetSlider == nullptr ) {
		return _min;
	}

	/** Return Value */
	return (_min + (_max - _min) * static_cast<double>(_widgetSlider->value()) / (_steps - 1));
}

void gui_tools::GuiUtil::QSliderWidget::setValue(double value)
{
	/** Sanity Check */
	if ( _widgetSlider == nullptr ) {
		return;
	}

	if ( value < _min || value > _max ) {
		throw std::runtime_error("Slider value out of range.");
	}

	/** Set Value */
	const int newValue = static_cast<int>(std::round((value - _min) * static_cast<double>(_steps - 1) / (_max - _min)));
	_widgetSlider->setValue(newValue);
	updateValueDisplay(newValue);
}

QSlider* gui_tools::GuiUtil::QSliderWidget::getSlider()
{
	return _widgetSlider;
}

void gui_tools::GuiUtil::QSliderWidget::updateValueDisplay(int value)
{
	/** Sanity Check */
	if ( _widgetLineEdit == nullptr ) {
		return;
	}

	/** Get Slider Value */
	const double numericNumber = _min + (_max - _min) * static_cast<double>(value) / (_steps - 1);

	/** Update Slider Text */
	_widgetLineEdit->setText(_prefix + QString::number(numericNumber, 'f', _precision) + _suffix);
}

void gui_tools::GuiUtil::QSliderWidget::setObjectNameOfWidgets(const QString& objectName)
{
	setObjectName(objectName);
	_widgetLabel->setObjectName(objectName);
	_widgetSlider->setObjectName(objectName);
	_widgetLineEdit->setObjectName(objectName);
}