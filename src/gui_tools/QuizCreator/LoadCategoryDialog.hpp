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
	class LoadCategoryDialog : public QDialog {
		Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit LoadCategoryDialog(const std::string& quizName, const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Default deconstructor.
		 */
		virtual ~LoadCategoryDialog() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		LoadCategoryDialog(const LoadCategoryDialog&) = delete;
		LoadCategoryDialog& operator=(const LoadCategoryDialog&) = delete;

	private slots:
		/**
		 * @brief Updates the table.
		 */
		void updateTable();

		/**
		 * @brief Emits the load quiz with the selected index.
		 */
		void loadCategory();

	signals:
		void loadSignal(const std::string&, const std::string&);

	private:
		/**
		 * @brief Constructs the widget layout.
		 */
		void makeWidgetLayout();

		/** Variables */
		std::string _quizName = "";
		QButtonGroup* _buttonGroup = nullptr;
		QTableWidget* _categoryTable = nullptr;

		std::vector<std::string> _categoryList;
		const common::Configuration& _config;
	};
}