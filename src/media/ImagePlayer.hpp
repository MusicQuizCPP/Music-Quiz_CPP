#pragma once

#include <memory>

#include <QLabel>
#include <QString>
#include <QWidget>
#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>



namespace media {
	class ImagePlayer : public QWidget {
		Q_OBJECT
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit ImagePlayer(QWidget* parent = nullptr);

		/**
		 * @brief destructor
		 */
		virtual ~ImagePlayer();

		/**
		 * @brief Shared Pointer
		 */
		typedef std::shared_ptr< ImagePlayer > Ptr;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		ImagePlayer(const ImagePlayer&) = delete;
		ImagePlayer& operator=(const ImagePlayer&) = delete;

		/**
		 * @brief Displays an image.
		 *
		 * @param[in] imageFile The name of the image file to show.
		 */
		void showImage(const QString& imageFile);

		/**
		 * @brief Resize the widget.
		 *
		 * @param[in] size The new size.
		 */
		void resize(const QSize& size);

		/**
		 * @brief Set the mouse event call back function.
		 *
		 * @param[in] mouseEventCallback The callback function.
		 */
		void setMouseEventCallbackFunction(const std::function< void(QMouseEvent*) > mouseEventCallback);

	private slots:

	protected:
		/**
		 * @brief Override the mouse release event.
		 *
		 * @param[in] event The event.
		 */
		void mousePressEvent(QMouseEvent* event) override;

		/**
		 * @brief Override the key press event.
		 *
		 * @param[in] event The event.
		 */
		void keyPressEvent(QKeyEvent* event) override;

		/** Variables */
		QLabel* _imageLabel = nullptr;

		std::function< void(QMouseEvent*) > _mouseEventCallback;
	};
}