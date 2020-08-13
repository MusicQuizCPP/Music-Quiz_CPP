#include "LightDeviceConnectedWidget.hpp"

#include <QPainter>

#include "common/Log.hpp"

#include "lightcontrol/client/LightControlClient.hpp"


using namespace MusicQuiz;
using namespace LightControl;

LightDeviceConnectedWidget::LightDeviceConnectedWidget(std::shared_ptr<LightControlClient> client, QWidget* parent) :
	QPushButton(parent), _client(client)
{
	_updater.setInterval(std::chrono::milliseconds(500));
	repaint();
	_updater.callOnTimeout(this, &LightDeviceConnectedWidget::updateState);
	_updater.start();
}

void LightDeviceConnectedWidget::updateState()
{
	repaint();
	if ( _client->isConnected() ) {
		setToolTip(_client->getConnectionString().c_str());
	} else {
		setToolTip("");
	}
}

void LightDeviceConnectedWidget::paintEvent(QPaintEvent* event)
{
	QPushButton::paintEvent(event);

	if ( _client->isRunning() ) {
		if ( _client->isConnected() ) {
			paintConnectedCircle(Qt::green);
		} else {
			paintConnectedCircle(Qt::red);
		}
	}
}

void LightDeviceConnectedWidget::paintConnectedCircle(const QColor color)
{
	QPainter painter(this);
	painter.setPen(color);
	painter.setBrush(color);
	painter.drawEllipse(QPointF(size().width() / 6, size().height() / 6), size().width() / 15, size().width() / 15);
}