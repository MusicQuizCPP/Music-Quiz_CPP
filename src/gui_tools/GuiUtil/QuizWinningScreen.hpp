#pragma once

#include <vector>

#include <QLabel>
#include <QTimer>
#include <QColor>
#include <QObject>
#include <QWidget>
#include <QDialog>

#include "QExtensions/ParticleSystem.hpp"
#include "QExtensions/ConfettiSystem.hpp"
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

	private slots:

		/**
		 * @brief Emits the winningScreenCompleteSignal signal
		 */
		void screenComplete();

	signals:
		void winningScreenCompleteSignal();

	protected:
		/**
		 * @brief Overrides the paint event.
		 *
		 * @param[in] event The event.
		 */
		void paintEvent(QPaintEvent* event) override;
		void resizeEvent(QResizeEvent*) override;

		QPixmap _noisePixmap;

		/** Variables */
		qreal _animationPhase = 0.0;
		ParticleSystem _particles;
		ConfettiSystem _confetti;

		QTimer _timeOutTimer;
		QTimer _animationTimer;
		const std::chrono::milliseconds _winnerDisplayTime;

		std::vector<MusicQuiz::QuizTeam*> _winningTeams;

		QString _winnerLabel;
		QFont _winnerLabelFittedFont;
		QRect _winnerLabelRect;

		QRadialGradient _backgroundColor;

		QString _winnerNames;
	};
}