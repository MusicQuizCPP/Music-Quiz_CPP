#pragma once

#include <vector>
#include <memory>

#include <QString>
#include <QObject>
#include <QWidget>

#include "gui_tools/GuiUtil/QExtensions/QPushButtonExtender.hpp"


namespace MusicQuiz {
	class QuizEntry;
	class QuizCategory : public QWidget {
		Q_OBJECT
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] name The category name.
		 * @param[in] entries The category entries.
		 * @param[in] parent The parent widget.
		 */
		explicit QuizCategory(QString name, const std::vector<MusicQuiz::QuizEntry*>& entries, QWidget* parent = nullptr);

		/**
		 * @brief Default Destructor
		 */
		virtual ~QuizCategory() = default;

		/**
		 * @brief Deleted the copy and assignment constructor.
		 */
		QuizCategory(const QuizCategory&) = delete;
		QuizCategory& operator=(const QuizCategory&) = delete;

		enum class CategoryState {
			IDLE, GUESSED
		};

		/**
		 * @brief Gets the size of the category.
		 *
		 * @return The size of the category.
		 */
		size_t getSize();

		/** Overload [] operator */
		MusicQuiz::QuizEntry*& operator[](size_t index)
		{
			return _entries[index];
		};
	public slots:
		/**
		 * @brief Enables guess the category.
		 *
		 * @param[in] enabled Enabled if true.
		 */
		void enableGuessTheCategory(size_t points);

		/**
		 * @brief Sets the color of the category label.
		 *
		 * @param[in] color The color to set.
		 */
		void setCategoryColor(const QColor& color);

		/**
		 * @brief Gets if the category has been guessed.
		 *
		 * @return True if the category has been guessed.
		 */
		bool hasCateogryBeenGuessed();

	private slots:
		/**
		 * @brief Handles the mouse right click event.
		 */
		void rightClickEvent();

		/**
		 * @brief Handles the mouse left click event.
		 */
		void leftClickEvent();

	signals:
		void guessed(size_t points);

	protected:
		/**
		 * @brief Creates the category layout.
		 */
		void createLayout();

		/** Variables */
		QString _name = "";
		size_t _points = 0;
		bool _guessTheCategory = false;
		CategoryState _state = CategoryState::IDLE;
		std::vector<MusicQuiz::QuizEntry*> _entries;
		MusicQuiz::QExtensions::QPushButtonExtender* _categoryBtn = nullptr;
	};
}