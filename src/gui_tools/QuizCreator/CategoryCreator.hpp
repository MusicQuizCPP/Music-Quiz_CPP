#pragma once 

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>

#include "media/AudioPlayer.hpp"
#include "gui_tools/GuiUtil/QExtensions/QTabWidgetExtender.hpp"

namespace common {
	class Configuration;
}

namespace MusicQuiz {
	class EntryCreator;

	class CategoryCreator : public QWidget {
		Q_OBJECT

	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] name The category name.
		 * @param[in] audioPlayer The audio player.
		 * @param[in] parent The parent widget.
		 */
		explicit CategoryCreator(const QString& name, const std::shared_ptr< media::AudioPlayer >& audioPlayer, const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Default destructor
		 */
		virtual ~CategoryCreator() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		CategoryCreator(const CategoryCreator&) = delete;
		CategoryCreator& operator=(const CategoryCreator&) = delete;

	public slots:
		/**
		 * @brief Updates the category name
		 *
		 * @param[in] name The name.
		 */
		void setName(const QString& name);

		/**
		 * @brief Gets the category name
		 *
		 * @return The name.
		 */
		const QString getName() const;

		/**
		 * @brief Sets the entries
		 *
		 * @param[in] entries The category entries.
		 */
		void setEntries(const std::vector< MusicQuiz::EntryCreator* >& entries);

		/**
		 * @brief Gets the entries.
		 *
		 * @return The category entries.
		 */
		const std::vector< MusicQuiz::EntryCreator* > getEntries() const;

		/**
		 * @brief Clears the entries.
		 */
		void clearEntries();

	private slots:
		/**
		 * @brief Adds a entry to the category.
		 */
		void addEntry();

		/**
		 * @brief Goes to the entry tab.
		 */
		void editEntry();

		/**
		 * @brief Removes a entry.
		 */
		void removeEntry();

		/**
		 * @brief Updates the name of the tab.
		 *
		 * param[in] str The new name.
		 */
		void updateEntryTabName(const QString& str);

		/**
		 * @brief Opens a dialog to select which quiz to load a category from.
		 */
		void openSelectQuizDialog();

		/**
		 * @brief Opens a dialog to select which category to load.
		 */
		void openSelectCategoryDialog(const std::string& quizName);

		/**
		 * @brief Loads a category.
		 *
		 * param[in] quizName The quiz name to load the category from.
		 * param[in] categoryName The category name to load.
		 */
		void loadCategory(const std::string& quizName, const std::string& categoryName);

	private:
		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/** Variables */
		QString _categoryName = "";
		QLabel* _categoryNameLabel = nullptr;

		QExtensions::QTabWidgetExtender* _tabWidget = nullptr;

		QTableWidget* _entriesTable = nullptr;
		std::vector< MusicQuiz::EntryCreator* > _entries;

		std::shared_ptr< media::AudioPlayer > _audioPlayer = nullptr;

		const common::Configuration& _config;
	};
}