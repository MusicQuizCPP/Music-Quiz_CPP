#pragma once

#include <string>
#include <vector>

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QKeyEvent>
#include <QListWidget>

#include "util/QuizLoader.hpp"
#include "util/QuizSettings.hpp"

namespace common {
	class Configuration;
}

namespace MusicQuiz {
	class QuizSettingsDialog;

	class QuizSelector : public QDialog {
		Q_OBJECT
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] parent The parent widget.
		 */
		explicit QuizSelector(const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Destructor
		 */
		virtual ~QuizSelector() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		QuizSelector(const QuizSelector&) = delete;
		QuizSelector& operator=(const QuizSelector&) = delete;

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
		 * @brief Updates the displayed information based on the quiz that is clicked.
		 *
		 * @param[in] idx The quiz index.
		 */
		void selectionClicked();

		/**
		 * @brief Emits the quiz selected signal.
		 */
		void quizSelected();

		/**
		 * @brief Opens the settings dialog.
		 */
		void openSettingsDialog();

		/**
		 * @brief Updates the quiz settings.
		 *
		 * @param[in] settings The quiz settings.
		 */
		void updateSettings(const MusicQuiz::QuizSettings& settings);

		/**
		 * @brief Emits the quit signal.
		 */
		void quit();

	signals:
		void quitSignal();
		void quizSelectedSignal(size_t quizIdx, const QString& quizName, const QString& quizAuthor, const MusicQuiz::QuizSettings& settings);
	protected:
		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/** Variables */
		bool _quizClosed = false;

		QLineEdit* _authorText = nullptr;
		QTextEdit* _categoryText = nullptr;
		QTextEdit* _rowCategoryText = nullptr;
		QTextEdit* _descriptionText = nullptr;
		QListWidget* _quizSelectionList = nullptr;

		QCheckBox* _includeSongsCheckbox = nullptr;
		QCheckBox* _includeVideosCheckbox = nullptr;
		QCheckBox* _guessTheCategoryCheckbox = nullptr;

		MusicQuiz::QuizSettings _settings;

		std::vector<std::string> _quizList;
		std::vector<MusicQuiz::util::QuizLoader::QuizPreview> _quizPreviews;
		const common::Configuration& _config;
	};
}
