#pragma once

#include <memory>
#include <filesystem>

#include <QLabel>
#include <QTimer>
#include <QString>
#include <QWidget>
#include <QObject>
#include <QKeyEvent>
#include <QHideEvent>
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
		virtual ~ImagePlayer() = default;

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
		 * @param[in] imageFile            The name of the image file to show.
		 * @param[in] depixilationDuration The durtation it takes for the image to go from pixilated to normal. If 0 the image will be shown as normal.
		 */
		void showImage(const std::filesystem::path& imageFile, const int depixilationDuration = 0);

		/**
		 * @brief Pauses the depixilation.
		 */
		void pause();

		/**
		 * @brief Resumes the depixilation.
		 */
		void resume();

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

	public slots:
		/**
		 * @brief Handles the close event.
		 *
		 * @param[in] event The event.
		 */
		void closeEvent(QCloseEvent* event);

	private slots:
		/**
		 * @brief Update the image pixilation.
		 */
		void updateImagePixilation();

	signals:
		void shown();
		void hidden();

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

		/**
		 * @brief Override the hide event.
		 *
		 * @param[in] event The event.
		 */
		void hideEvent(QHideEvent* event) override;

		/** The image label */
		QLabel* _imageLabel = nullptr;

		/** The mouse event callback function */
		std::function< void(QMouseEvent*) > _mouseEventCallback;

		/** The update timer */
		QTimer* _updateTimer = nullptr;

		/** The depixilation duration in msec */
		int _depixilationDuration = 0;

		/** The elapsed time in msec */
		int _elapsedTime = 0;

		/** Original pixmap */
		QPixmap _originalPixmap;
	};
}