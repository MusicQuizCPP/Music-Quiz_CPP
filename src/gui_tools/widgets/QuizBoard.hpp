#pragma once

#include <vector>
#include <string>
#include <memory>

#include <QColor>
#include <QString>
#include <QObject>
#include <QDialog>
#include <QKeyEvent>

#include "util/QuizSettings.hpp"
#include "lightcontrol/client/LightControlClient.hpp"

namespace MusicQuiz {
	class QuizTeam;
	class QuizCategory;
	class QuizBoard : public QDialog {
		Q_OBJECT
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] categories The categories.
		 * @param[in] rowCategories The row categories.
		 * @param[in] teams The list of teams.
		 * @param[in] settings The quiz settings.
		 * @param[in] preview If the quiz board should be shown in preview mode.
		 * @param[in] parent The parent widget.
		 */
		explicit QuizBoard(const std::vector<MusicQuiz::QuizCategory*>& categories, const std::vector<QString>& rowCategories,
			const std::vector<MusicQuiz::QuizTeam*>& teams, const MusicQuiz::QuizSettings& settings, bool preview = false, QWidget* parent = nullptr);

		/**
		 * @brief Default Destructor
		 */
		virtual ~QuizBoard() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		QuizBoard(const QuizBoard&) = delete;
		QuizBoard& operator=(const QuizBoard&) = delete;

		/**
		 * @brief Sets the quiz name.
		 *
		 * @param[in] name The quiz name.
		 */
		void setQuizName(const QString& name);

		/**
		 * @brief Get the quiz name.
		 *
		 * @return The quiz name.
		 */
		QString getQuizName();

		/**
		 * @brief callback to run when the light client connects.
		 */
		static void lightClientConnectedCallback(LightControl::LightControlClient* client);

	public slots:
		/**
		 * @brief Closes the window.
		 *
		 * @return True if the window should be closed.
		 */
		bool closeWindow();

		/**
		 * @brief Handle answer.
		 *
		 * @param[in] points The points from the entry.
		 */
		void handleAnswer(size_t points);

		/**
		 * @brief Checks if game is over.
		 */
		void handleGameComplete();

		/**
		 * @brief Handles the close event.
		 *
		 * @param[in] event The event.
		 */
		void closeEvent(QCloseEvent* event);

		/**
		 * @brief Handles the key press events.
		 *
		 * @param[in] event The event.
		 */
		void keyPressEvent(QKeyEvent* event);

		/**
		 * @brief Event filter used to grab the esc key press events.
		 *
		 * @param[in] target The target.
		 * @param[in] event The event.
		 */
		bool eventFilter(QObject* target, QEvent* event);

	signals:
		void quitSignal();
		void gameComplete(std::vector<MusicQuiz::QuizTeam*> winningTeam);

	protected:
		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/** Variables */
		bool _quizClosed = false;
		bool _quizStopped = false;

		QString _name = "";

		MusicQuiz::QuizSettings _settings;

		std::vector<QuizTeam*> _teams;
		std::vector<QString> _rowCategories;
		std::vector<MusicQuiz::QuizCategory*> _categories;
		std::shared_ptr<LightControl::LightControlClient> _lightClient;
	};
}