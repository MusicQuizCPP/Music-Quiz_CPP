#pragma once

#include <vector>
#include <chrono>
#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QDialog>


namespace MusicQuiz {
	class QuizIntroScreen : public QDialog {
		Q_OBJECT
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] quizName The quiz name.
		 * @param[in] quizAuhtor The quiz author.
		 * @param[in] parent The parent widget.
		 */
		explicit QuizIntroScreen(const QString& quizName, const QString& quizAuhtor, QWidget* parent = nullptr);

		/**
		 * @brief Destructor
		 */
		virtual ~QuizIntroScreen();

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		QuizIntroScreen(const QuizIntroScreen&) = delete;
		QuizIntroScreen& operator=(const QuizIntroScreen&) = delete;

	public slots:

	private slots:
		/**
		 * @brief Emits the introCompleteSignal signal
		 */
		void introComplete();

	signals:
		void introCompleteSignal();
	protected:
		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/** Variables */
		QString _quizName = "";
		QString _quizAuthor = "";

		QTimer _timer;
		const std::chrono::milliseconds _introDisplayTime;
	};
}