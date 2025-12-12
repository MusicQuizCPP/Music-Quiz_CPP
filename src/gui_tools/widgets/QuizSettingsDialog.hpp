#pragma once

#include <string>
#include <vector>

#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QSlider>
#include <QLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QKeyEvent>
#include <QGridLayout>

#include "util/QuizSettings.hpp"

#include "lightcontrol/discover/LightControlDiscover.hpp"


namespace MusicQuiz {
	class QuizSettingsDialog : public QDialog {
		Q_OBJECT

	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] settings The quiz settings.
		 * @param[in] parent The parent widget.
		 */
		explicit QuizSettingsDialog(const MusicQuiz::QuizSettings& settings, QWidget* parent = nullptr);

		/**
		 * @brief Destructor
		 */
		virtual ~QuizSettingsDialog() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		QuizSettingsDialog(const QuizSettingsDialog&) = delete;
		QuizSettingsDialog& operator=(const QuizSettingsDialog&) = delete;

	public slots:
		/**
		 * @brief Handles the key press events.
		 *
		 * @param[in] event The event.
		 */
		void keyPressEvent(QKeyEvent* event);

	private slots:
		/**
		 * @brief Enables / diables the guess time limit settings.
		 *
		 * @param[in] enabled If true the guess time limit settings will be enabled.
		 */
		void setGuessTimeLimitEnabled(bool enabled);

		/**
		 * @brief Updates the guess time limit line edit.
		 *
		 * @param[in] value The new value.
		 */
		void setGuessTimeLimitTime(int value);

		/**
		 * @brief Enables / diables the bingo settings.
		 *
		 * @param[in] enabled If true the bingo settings will be enabled.
		 */
		void setBingoEnabled(bool enabled);

		/**
		 * @brief Enables / diables the daily double settings.
		 *
		 * @param[in] enabled If true the daily double settings will be enabled.
		 */
		void setDailyDoubleEnabled(bool enabled);

		/**
		 * @brief Enables / diables the daily triple settings.
		 *
		 * @param[in] enabled If true the daily triple settings will be enabled.
		 */
		void setDailyTripleEnabled(bool enabled);

		/**
		 * @brief Updates the daily double line edit.
		 *
		 * @param[in] value The new value.
		 */
		void setDailyDoublePercentage(int value);

		/**
		 * @brief Updates the daily triple line edit.
		 *
		 * @param[in] value The new value.
		 */
		void setDailyTriplePercentage(int value);

		/**
		 * @brief Closes the dialog and emits a signal with the settings.
		 */
		void saveSettings();

		/**
		 * @brief Emits the quit signal.
		 */
		void quit();

		/**
		 * @brief Show a popup information message box.
		 *
		 * @param[in] info The information to be displayed within the message box.
		 */
		void informationMessageBox(const QString& info);

		/**
		 * @brief Update list of discovered light devices.
		 *
		 */
		void updateLightDevices();

		/**
		 * @brief Update currently inputted IP.
		 *
		 */
		void updateIP(int index);

		/**
		 * @brief Display Information boxes for the different settings.
		 */
		void showHiddenTeamsInfo();
		void showHiddenAnswersInfo();
		void showGuessTimeLimitInfo();
		void showBingoInfo();
		void showDailyDoubleInfo();
		void showDailyTripleInfo();
		void showDailyDoubleHiddenInfo();
		void showDailyTripleHiddenInfo();

	signals:
		void quitSignal();
		void settingsUpdated(MusicQuiz::QuizSettings settings);

	protected:
		/**
		 * @brief Creates the category layout.
		 *
		 * @param[in] settings The currently set settings.
		 */
		void createLayout(const MusicQuiz::QuizSettings& settings);

		/**
		 * @brief Creates the guess time limit layout.
		 *
		 * @param[in] settings The currently set settings.
		 *
		 * @return The guess time limit  layout.
		 */
		QWidget* getGuessTimeLimitLayout(const MusicQuiz::QuizSettings& settings);

		/**
		 * @brief Creates the bingo layout.
		 *
		 * @param[in] settings The currently set settings.
		 *
		 * @return The bingo layout.
		 */
		QWidget* getBingoLayout(const MusicQuiz::QuizSettings& settings);

		/**
		 * @brief Creates the daily double layout.
		 *
		 * @param[in] settings The currently set settings.
		 *
		 * @return The daily double layout.
		 */
		QWidget* getDailyDoubleLayout(const MusicQuiz::QuizSettings& settings);

		/**
		 * @brief Creates the daily triple layout.
		 *
		 * @param[in] settings The currently set settings.
		 *
		 * @return The daily triple layout.
		 */
		QWidget* getDailyTripleLayout(const MusicQuiz::QuizSettings& settings);

		/**
		 * @brief Creates the Light Device interface.
		 *
		 * @param[in] settings The currently set settings.
		 *
		 * @return The Light Device layout.
		 */
		QWidget* getLightInterfaceLayout(const MusicQuiz::QuizSettings& settings);

		/**
		 * @brief Enable / disable a layout and its children.
		 *
		 * @param[in] layout The layout.
		 * @param[in] enabled If true the layout are enabled.
		 */
		void setLayoutEnabled(QLayout* layout, bool enabled);

		/** Variables */
		QCheckBox* _hiddenTeam = nullptr;
		QCheckBox* _hiddenAnswers = nullptr;

		/** Guess Time Limit */
		QCheckBox* _guessTimeLimit = nullptr;
		QSlider* _guessTimeLimitSlider = nullptr;
		QGridLayout* _guessTimeLimitLayout = nullptr;
		QLineEdit* _guessTimeLimitLineEdit = nullptr;
		const int _minGuessTimeLimit = 5, _maxGuessTimeLimit = 30;

		/** Daily Double */
		QCheckBox* _dailyDouble = nullptr;
		QCheckBox* _dailyDoubleHidden = nullptr;
		QSlider* _dailyDoublePercentage = nullptr;
		QGridLayout* _dailyDoubleLayout = nullptr;
		QLineEdit* _dailyDoublePercentageLineEdit = nullptr;
		const int _minDoublePercentage = 5, _maxDoublePercentage = 75;

		/** Daily Triple */
		QCheckBox* _dailyTriple = nullptr;
		QCheckBox* _dailyTripleHidden = nullptr;
		QSlider* _dailyTriplePercentage = nullptr;
		QGridLayout* _dailyTripleLayout = nullptr;
		QLineEdit* _dailyTriplePercentageLineEdit = nullptr;
		const int _minTriplePercentage = 5, _maxTriplePercentage = 25;

		/**LightInterface */
		QTimer _listUpdateTimer;
		LightControl::LightControlDiscover lightcontrolDiscover;
		QLineEdit* _ipInput = nullptr;
		QComboBox* _discoveredList = nullptr;

		/** Bingo */
		QCheckBox* _bingoEnabledCheckbox = nullptr;
		QSpinBox* _bingoPointsSpinbox = nullptr;
		QHBoxLayout* _bingoLayout = nullptr;
	};
}