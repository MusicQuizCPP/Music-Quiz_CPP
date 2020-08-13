#pragma once

#include <vector>

#include <QLabel>
#include <QTimer>
#include <QColor>
#include <QObject>
#include <QWidget>
#include <QDialog>

#include "gui_tools/widgets/QuizTeam.hpp"


namespace MusicQuiz {
	class QuizWinningScreen : public QDialog {
		Q_OBJECT
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] winningTeams The list of teams that won.
		 * @param[in] parent The parent widget.
		 */
		explicit QuizWinningScreen(const std::vector<MusicQuiz::QuizTeam*>& winningTeams, QWidget* parent = nullptr);

		/**
		 * @brief Destructor
		 */
		virtual ~QuizWinningScreen();

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		QuizWinningScreen(const QuizWinningScreen&) = delete;
		QuizWinningScreen& operator=(const QuizWinningScreen&) = delete;

	public slots:

	private slots:
		/**
		 * @brief Increase the size of the team name.
		 */
		void increaseTextSize();

		/**
		 * @brief Emits the winningScreenCompleteSignal signal
		 */
		void screenComplete();

	signals:
		void winningScreenCompleteSignal();

	protected:
		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/** Variables */
		int _textSize = 1;
		int _hueCounter = 0;
		QColor _textColor = QColor(255, 255, 0);

		QTimer _timer;
		QTimer _sizeTimer;
		const std::chrono::milliseconds _winnerDisplayTime;

		std::vector< QLabel* > _andLabels;
		std::vector< QLabel* > _winnersLabels;
		std::vector<MusicQuiz::QuizTeam*> _winningTeams;
	};
}