#pragma once

#include <string>
#include <vector>

#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QButtonGroup>

class QTableWidget;

namespace common {
	class Configuration;
}

namespace MusicQuiz {
	/**
	 * @brief Load Dialog.
	 */
	class LoadDialog : public QDialog {
		Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit LoadDialog(QWidget* parent = nullptr);

		/**
		 * @brief Default deconstructor.
		 */
		virtual ~LoadDialog() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		LoadDialog(const LoadDialog&) = delete;
		LoadDialog& operator=(const LoadDialog&) = delete;

		/**
		 * @brief Updates the table.
		 */
		virtual void updateTable() = 0;

	private slots:
		/**
		 * @brief Called when load is pressed
		 */
		virtual void load() = 0;

    protected:
		QTableWidget* _table = nullptr;
		QButtonGroup* _buttonGroup = nullptr;
        
	private:
		/**
		 * @brief Constructs the widget layout.
		 */
		void makeWidgetLayout();
	};
}