#pragma once

#include <string>
#include <vector>

#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QButtonGroup>

#include "LoadDialog.hpp"

class QTableWidget;

namespace common {
	class Configuration;
}

namespace MusicQuiz {
	/**
	 * @brief Category Load Dialog.
	 */
	class LoadCategoryDialog : public LoadDialog {
		Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit LoadCategoryDialog(const common::Configuration& config, QWidget* parent = nullptr) : LoadDialog(parent), _config(config) {}

		/**
		 * @brief Default deconstructor.
		 */
		virtual ~LoadCategoryDialog() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		LoadCategoryDialog(const LoadCategoryDialog&) = delete;
		LoadCategoryDialog& operator=(const LoadCategoryDialog&) = delete;

		/**
		 * @brief Updates the table.
		 */
		virtual void updateTable();

	private slots:

		/**
		 * @brief Called when load is pressed.
		 */
		virtual void load();


	signals:
		void loadSignal(const std::string& quiz, const std::string& category);

	private:

		std::vector<std::pair<std::string, std::string> > _categoryList;
		const common::Configuration& _config;
	};
}