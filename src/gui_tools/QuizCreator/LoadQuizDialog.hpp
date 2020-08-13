#pragma once

#include <string>
#include <vector>

#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QButtonGroup>

#include "util/QuizLoader.hpp"

class QTableWidget;

namespace common {
	class Configuration;
}

namespace MusicQuiz {
	/**
	 * @brief Quiz Load Dialog.
	 */
	class LoadQuizDialog : public QDialog {
		Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit LoadQuizDialog(const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Default deconstructor.
		 */
		virtual ~LoadQuizDialog() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		LoadQuizDialog(const LoadQuizDialog&) = delete;
		LoadQuizDialog& operator=(const LoadQuizDialog&) = delete;

	private slots:
		/**
		 * @brief Updates the table.
		 */
		void updateTable();

		/**
		 * @brief Emits the load quiz with the selected index.
		 */
		void loadQuiz();

	signals:
		void loadSignal(const std::string&);

	private:
		/**
		 * @brief Constructs the widget layout.
		 */
		void makeWidgetLayout();

		/** Variables */
		QTableWidget* _quizTable = nullptr;
		QButtonGroup* _buttonGroup = nullptr;

		std::vector<std::string> _quizList;
		const common::Configuration& _config;
	};
}