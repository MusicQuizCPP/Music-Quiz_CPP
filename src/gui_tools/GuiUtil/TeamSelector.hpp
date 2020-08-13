#pragma once

#include <string>
#include <vector>

#include <QColor>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QKeyEvent>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>

#include "gui_tools/widgets/QuizTeam.hpp"
#include "gui_tools/GuiUtil/ColorPicker/QHueSlider.hpp"


namespace MusicQuiz {
	class TeamSelector : public QDialog {
		Q_OBJECT
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit TeamSelector(QWidget* parent = nullptr);

		/**
		 * @brief Destructor
		 */
		virtual ~TeamSelector() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		TeamSelector(const TeamSelector&) = delete;
		TeamSelector& operator=(const TeamSelector&) = delete;

	public slots:
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

	private slots:
		/**
		 * @brief Closes the window.
		 *
		 * @return True if the window should be closed.
		 */
		bool closeWindow();

		/**
		 * @brief Emits the quit signal.
		 */
		void quit();

		/**
		 * @brief Emits the quiz selected signal.
		 */
		void teamSelected();

		/**
		 * @brief Adds a team to the team table.
		 */
		void addTeam();

		/**
		 * @brief Removes the selected team from the team table.
		 */
		void removeTeam();

		/**
		 * @brief Updates the color indicator.
		 *
		 * @param[in] color The selected color.
		 */
		void teamColorChanged(QColor color);

	signals:
		void quitSignal();
		void teamSelectedSignal(const std::vector<MusicQuiz::QuizTeam*>&);
	protected:
		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/**
		 * @brief Sets the team number of all teams
		 */
		void setTeamNumbers();

		/** Variables */
		bool _quizClosed = false;
		const size_t _maximumsNumberOfTeams = 6;

		QTableWidget* _teamTable = nullptr;
		QLineEdit* _teamNameLineEdit = nullptr;
		QColor _currentColor = QColor(0, 0, 255);

		ColorPicker::QHueSlider* _hueSlider = nullptr;
	};
}