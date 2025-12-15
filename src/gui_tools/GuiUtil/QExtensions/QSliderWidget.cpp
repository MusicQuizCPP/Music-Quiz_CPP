#include "QSliderWidget.hpp"

#include <cmath>
#include <stdexcept>

#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QGridLayout>


gui_tools::GuiUtil::QSliderWidget::QSliderWidget(const QString& title, double min, double max, int steps, const QString& prefix, const QString& suffix, int prec, QWidget* parent) :
	QFrame(parent), title_(title), min_(min), max_(max), steps_(steps), prefix_(prefix), suffix_(suffix), precision_(prec)
{
	/** Sanity Check */
	if ( steps_ < 2 ) {
		throw std::runtime_error("Can not have less than two steps in the slider.");
	}

	if ( max_ <= min_ ) {
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
	widgetLabel_ = new QLabel(title_);
	widgetLabel_->setObjectName("quizCreatorLabel");
	sliderLayout->addWidget(widgetLabel_, 0, 0, Qt::AlignLeft);

	/** LineEdit */
	widgetLineEdit_ = new QLineEdit(prefix_ + QString::number(min_, 'f', precision_) + suffix_);
	widgetLineEdit_->setObjectName("quizCreatorSliderLineEdit");
	widgetLineEdit_->setAlignment(Qt::AlignRight);
	widgetLineEdit_->setReadOnly(true);
	sliderLayout->addWidget(widgetLineEdit_, 0, 1, Qt::AlignRight);

	/** Slider */
	widgetSlider_ = new QSlider(Qt::Horizontal, this);
	widgetSlider_->setRange(0, steps_ - 1);
	widgetSlider_->setSingleStep(1);
	widgetSlider_->setPageStep(1);
	widgetSlider_->setValue(0);
	widgetSlider_->setObjectName("quizCreatorSlider");
	QObject::connect(widgetSlider_, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
	QObject::connect(widgetSlider_, SIGNAL(valueChanged(int)), this, SLOT(updateValueDisplay(int)));
	sliderLayout->addWidget(widgetSlider_, 1, 0, 1, 2);

	/** Set Layout */
	setLayout(sliderLayout);
}

double gui_tools::GuiUtil::QSliderWidget::getValue() const
{
	/** Sanity Check */
	if ( widgetSlider_ == nullptr ) {
		return min_;
	}

	/** Return Value */
	return (min_ + (max_ - min_) * static_cast<double>(widgetSlider_->value()) / (steps_ - 1));
}

void gui_tools::GuiUtil::QSliderWidget::setValue(double value)
{
	/** Sanity Check */
	if ( widgetSlider_ == nullptr ) {
		return;
	}

	if ( value < min_ || value > max_ ) {
		throw std::runtime_error("Slider value out of range.");
	}

	/** Set Value */
	const int newValue = static_cast<int>(std::round((value - min_) * static_cast<double>(steps_ - 1) / (max_ - min_)));
	widgetSlider_->setValue(newValue);
	updateValueDisplay(newValue);
}

QSlider* gui_tools::GuiUtil::QSliderWidget::getSlider()
{
	return widgetSlider_;
}

void gui_tools::GuiUtil::QSliderWidget::updateValueDisplay(int value)
{
	/** Sanity Check */
	if ( widgetLineEdit_ == nullptr ) {
		return;
	}

	/** Get Slider Value */
	const double numericNumber = min_ + (max_ - min_) * static_cast<double>(value) / (steps_ - 1);

	/** Update Slider Text */
	widgetLineEdit_->setText(prefix_ + QString::number(numericNumber, 'f', precision_) + suffix_);
}

void gui_tools::GuiUtil::QSliderWidget::setObjectNameOfWidgets(const QString& objectName)
{
	setObjectName(objectName);
	widgetLabel_->setObjectName(objectName);
	widgetSlider_->setObjectName(objectName);
	widgetLineEdit_->setObjectName(objectName);
}