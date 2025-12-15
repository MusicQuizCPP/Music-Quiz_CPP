#pragma once

#include <memory>

#include <QString>
#include <QWidget>
#include <QObject>
#include <QKeyEvent>
#include <QHideEvent>
#include <QMouseEvent>

#include "gui_tools/GuiUtil/QExtensions/QLabelExtender.hpp"



namespace media {
	class TextPlayer : public QWidget {
		Q_OBJECT

	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit TextPlayer(QWidget* parent = nullptr);

		/**
		 * @brief destructor
		 */
		virtual ~TextPlayer() = default;

		/**
		 * @brief Shared Pointer
		 */
		typedef std::shared_ptr< TextPlayer > Ptr;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		TextPlayer(const TextPlayer&) = delete;
		TextPlayer& operator=(const TextPlayer&) = delete;

		/**
		 * @brief Displays some text.
		 *
		 * @param[in] textString The text string to show.
		 */
		void showText(const QString& textString);

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

		/** Variables */
		MusicQuiz::QExtensions::QLabelExtender* _textLabel = nullptr;

		std::function< void(QMouseEvent*) > _mouseEventCallback;
	};
}