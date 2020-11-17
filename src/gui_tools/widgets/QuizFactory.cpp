#include "QuizFactory.hpp"

#include <math.h>
#include <time.h>
#include <vector>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdexcept>
#include <system_error>
#include <exception>

#include <QString>
#include <QMessageBox>

#include <boost/property_tree/xml_parser.hpp>

#include "common/Log.hpp"
#include "common/TimeUtil.hpp"
#include "common/Configuration.hpp"

#include "util/QuizLoader.hpp"
#include "gui_tools/widgets/QuizEntry.hpp"
#include "gui_tools/widgets/QuizCategory.hpp"
#include "gui_tools/QuizCreator/CategoryCreator.hpp"

using namespace std;

MusicQuiz::QuizBoard* MusicQuiz::QuizFactory::createQuiz(const string& quizName, const QuizSettings& settings, const media::AudioPlayer::Ptr& audioPlayer,
	const media::VideoPlayer::Ptr& videoPlayer, const common::Configuration& config, const vector<MusicQuiz::QuizTeam*>& teams, bool preview, QWidget* parent)
{
	/** Get List of Quizzes */
	vector<string> quizList = MusicQuiz::util::QuizLoader::getListOfQuizzes(config);
	if ( quizList.empty() ) {
		throw runtime_error("No quizzes found in the data folder.");
	}

	/** Check if Quiz Exists */
	size_t idx = 0;
	bool quizExists = false;
	for ( size_t i = 0; i < quizList.size(); ++i ) {
		replace(quizList[i].begin(), quizList[i].end(), '\\', '/');
		if ( quizName == quizList[i] ) {
			idx = i;
			quizExists = true;
			break;
		}
	}

	/** Sanity Check */
	if ( !quizExists ) {
		throw runtime_error("Quiz does not exists.");
	}

	/** Create Quiz */
	return createQuiz(idx, settings, audioPlayer, videoPlayer, config, teams, preview, parent);
}

MusicQuiz::QuizBoard* MusicQuiz::QuizFactory::createQuiz(const size_t idx, const QuizSettings& settings, const media::AudioPlayer::Ptr& audioPlayer,
	const media::VideoPlayer::Ptr& videoPlayer, const common::Configuration& config, const vector<MusicQuiz::QuizTeam*>& teams, bool preview, QWidget* parent)
{
	/** Seed Rand */
	srand(static_cast<unsigned int>(time(NULL)));

	/** Create Quiz Board */
	MusicQuiz::QuizBoard* quizBoard = nullptr;

	/** Load Categories */
	string loadError;
	vector<MusicQuiz::QuizCategory*> categories = MusicQuiz::util::QuizLoader::loadQuizCategories(idx, audioPlayer, videoPlayer, config, loadError);
	if ( !loadError.empty() ) {
		QMessageBox::information(nullptr, "Info", "Incomplete Quiz:\n\n" + QString::fromStdString(loadError));
	}

	/** Load Row Categories */
	vector< QString > rowCategories = MusicQuiz::util::QuizLoader::loadQuizRowCategories(idx, config);

	/** Hidden Team Score */
	if ( settings.hiddenTeamScore ) {
		for ( size_t i = 0; i < teams.size(); ++i ) {
			teams[i]->setHideScore(true);
		}
	}

	/** Get Number of Entries */
	size_t numberOfEntries = 0;
	for ( size_t i = 0; i < categories.size(); ++i ) {
		numberOfEntries += categories[i]->getSize();
	}

	vector<size_t> entriesIndicies(numberOfEntries);
	for ( size_t i = 0; i < numberOfEntries; ++i ) {
		entriesIndicies[i] = i;
	}

	/** Get Daily Double Entries */
	size_t dailyDoubleCount = static_cast<size_t>(std::floor(double(numberOfEntries * settings.dailyDoublePercentage) / 100.0));
	vector<size_t> dailyDoubleSelectedElements;
	if ( settings.dailyDouble && !teams.empty() ) {
		/** Ensure that there is atleas one element if the setting is enabled */
		if ( dailyDoubleCount == 0 ) {
			dailyDoubleCount = 1;
		}

		/** Select Elements */
		while ( dailyDoubleSelectedElements.size() < dailyDoubleCount && !entriesIndicies.empty() ) {
			const size_t randomIdx = rand() % entriesIndicies.size();
			dailyDoubleSelectedElements.push_back(entriesIndicies[randomIdx]);
			entriesIndicies.erase(entriesIndicies.begin() + randomIdx);
		}
	}

	/** Get Daily Triple Entries */
	size_t dailyTripleCount = static_cast<size_t>(std::floor(double(numberOfEntries * settings.dailyTriplePercentage) / 100.0));
	vector<size_t> dailyTripleSelectedElements;
	if ( settings.dailyTriple && !teams.empty() ) {

		/** Select Elements */
		while ( dailyTripleSelectedElements.size() < dailyTripleCount && !entriesIndicies.empty() ) {
			const size_t randomIdx = rand() % entriesIndicies.size();
			dailyTripleSelectedElements.push_back(entriesIndicies[randomIdx]);
			entriesIndicies.erase(entriesIndicies.begin() + randomIdx);
		}
	}

	/** Apply Settings */
	size_t counter = 0;
	for ( size_t i = 0; i < categories.size(); ++i ) {
		if ( settings.guessTheCategory ) {
			categories[i]->enableGuessTheCategory(settings.pointsPerCategory);
		}
		for ( size_t j = 0; j < categories[i]->getSize(); ++j ) {
			MusicQuiz::QuizEntry* quizEntry = (*categories[i])[j];
			if ( quizEntry != nullptr ) {
				/** Double Points */
				for ( size_t k = 0; k < dailyDoubleSelectedElements.size(); ++k ) {
					if ( counter == dailyDoubleSelectedElements[k] ) {
						quizEntry->setDoublePointsEnabled(true, settings.dailyDoubleHidden);
					}

				}

				/** Triple Points */
				for ( size_t k = 0; k < dailyTripleSelectedElements.size(); ++k ) {
					if ( counter == dailyTripleSelectedElements[k] ) {
						quizEntry->setTriplePointsEnabled(true, settings.dailyTripleHidden);
					}

				}

				/** Hidden Answers */
				quizEntry->setHiddenAnswer(settings.hiddenAnswers);
			}
			++counter;
		}
	}

	/** Create Quiz */
	quizBoard = new MusicQuiz::QuizBoard(categories, rowCategories, teams, settings, preview, parent);

	return quizBoard;
}