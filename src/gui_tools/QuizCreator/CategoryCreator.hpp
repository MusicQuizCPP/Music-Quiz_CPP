#pragma once 

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>

#include <boost/property_tree/ptree.hpp>

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
		 * @param[in] config configuration.
		 * @param[in] parent The parent widget.
		 */
		explicit CategoryCreator(const QString& name, const std::shared_ptr< media::AudioPlayer >& audioPlayer, 
			const common::Configuration& config, QWidget* parent = nullptr);

		/**
		 * @brief Constructor
		 *
		 * @param[in] tree property_tree to load category from.
		 * @param[in] audioPlayer The audio player.
		 * @param[in] config configuration.
		 * @param[in] skipEntries whether entries loading should be skipped.
		 * @param[in] parent The parent widget.
		 */
		explicit CategoryCreator(const boost::property_tree::ptree &tree, const media::AudioPlayer::Ptr& audioPlayer, const common::Configuration& config, 
			bool skipEntries = false, QWidget* parent = nullptr);
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
		 * @brief Check if entries has unique names.
		 *
		 * @return if all entry names are unique.
		 */
		bool areEntryNamesUnique() const;

		/**
		 * @brief Clears the entries.
		 */
		void clearEntries();

		/**
		 * @brief serialize category into a boost property_tree
		 *
		 * @param[in] savePath path where the mediafiles should be saved.
		 * @param[in] xmlPath path for the media files that should be written in the ptree.
		 * 
		 * @return the serialized ptree
		 */

		boost::property_tree::ptree saveToXml(const std::string savePath, const std::string& xmlPath);

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