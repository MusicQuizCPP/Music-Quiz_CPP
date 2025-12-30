#pragma once

#include <vector>
#include <string>
#include <memory>

#include <QColor>
#include <QString>
#include <QObject>
#include <QDialog>
#include <QKeyEvent>
#include <QShowEvent>
#include <QPushButton>

#include "util/QuizSettings.hpp"

#include "lightcontrol/client/LightControlClient.hpp"

#include "gui_tools/widgets/QuizTeam.hpp"
#include "gui_tools/widgets/QuizEntry.hpp"
#include "gui_tools/GuiUtil/QExtensions/QCountDownClock.hpp"
#include "gui_tools/GuiUtil/QExtensions/CategoryPushButtonExtender.hpp"


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
		 * @brief Checks if a bingo was completed.
		 * 
		 * @param[in] entry The quiz entry that was answered.
		 * @param[in] team  The team that answered the entry.
		 */
		void handleBingo(QuizEntry* entry, QuizTeam* team);

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

		/**
		 * @brief Blurs the quiz (used when displaying image or text).
		 */
		void blurQuiz();

		/**
		 * @brief Unblurs the quiz (used when displaying image or text).
		 */
		void unBlurQuiz();

		/**
		 * @brief Displays a countdown clock to show how much time the participant have left to guess.
		 */
		void startCountdown();

		/**
		 * @brief Stops the countdown.
		 */
		void stopCountdown();

	signals:
		void quitSignal();
		void gameComplete(std::vector<MusicQuiz::QuizTeam*> winningTeam);

	protected:
		/**
		 * @brief Overrides the show event to update the text size.
		 * 
		 * @param[in] event The event.
		 */
		void showEvent(QShowEvent* event) override;

		/**
		 * @brief Overrides the paint event.
		 *
		 * @param[in] event The event.
		 */
		void paintEvent(QPaintEvent* event) override;

		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/** Variables */
		bool _quizClosed = false;
		bool _quizStopped = false;

		QString _name = "";

		QuizSettings _settings;

		std::vector<QuizTeam*> _teams;
		std::vector<QString> _rowCategories;
		std::vector<QExtensions::CategoryPushButtonExtender*> _rowCategoryButtons;
		std::vector<QuizCategory*> _categories;

		/*/ Bingo tracking */
		std::vector< std::vector<int> > _cellOwner;
		std::vector< std::vector<bool> > _bingoRowAwarded; // [team][row]
		std::vector< std::vector<bool> > _bingoColAwarded; // [team][col]
		std::vector< std::vector<bool> > _bingoDiagAwarded; // [team][diag(0..1)]
		QPixmap _bingoPixmap = QPixmap(":/imgs/bingo.png");

		std::shared_ptr<LightControl::LightControlClient> _lightClient;

		QExtensions::QCountDownClock* _countdownClock = nullptr;

		const QPixmap _background = QPixmap(":imgs/background.png");
	};
}