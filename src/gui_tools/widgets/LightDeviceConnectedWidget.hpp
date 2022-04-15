#include <memory>
#include <QTimer>
#include <QPushButton> 

namespace LightControl {
	class LightControlClient;
}

namespace MusicQuiz {
	class LightDeviceConnectedWidget : public QPushButton {
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] client Light client.
		 * @param[in] parent parent widget.
		 */
		LightDeviceConnectedWidget(std::shared_ptr<LightControl::LightControlClient> client, QWidget* parent = nullptr);

	private:
		/**
		 * @brief Overwrite paintEvent.
		 *
		 * @param[in] event The event.
		 */
		void paintEvent(QPaintEvent* event);

		/**
		 * @brief Paint a circle in the corner of the widget.
		 *
		 * @param[in] color color of circle.
		 */
		void paintConnectedCircle(const QColor color);

		/**
		 * @brief Update the widget base on the client state.
		 */
		void updateState();

		/** The Light Controller Client */
		std::shared_ptr<LightControl::LightControlClient> _client;

		/** Update Timer */
		QTimer _updater;
	};
}