#include "QuizFactory.hpp"

#include <math.h>
#include <time.h>
#include <vector>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdexcept>
#include <system_error>

#include <QString>
#include <QMessageBox>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "common/Log.hpp"
#include "common/TimeUtil.hpp"
#include "common/Configuration.hpp"

#include "util/QuizLoader.hpp"
#include "gui_tools/widgets/QuizEntry.hpp"
#include "gui_tools/widgets/QuizCategory.hpp"
#include "gui_tools/QuizCreator/EntryCreator.hpp"
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

void MusicQuiz::QuizFactory::saveQuiz(const MusicQuiz::QuizCreator::QuizData& data, const common::Configuration& config, QWidget* parent)
{
	/** Tmp Folder */
	string mediaDirectoryPath;

	try {
		/** Get Quiz Name */
		const string quizName = data.quizName.toStdString();
		if ( quizName.empty() ) {
			QMessageBox::warning(parent, "Failed to Save Quiz", "The quiz name needs to be set before saving.");
			return;
		}

		/** Get Quiz Name */
		const string quizAuthor = data.quizAuthor.toStdString();
		if ( quizAuthor.empty() ) {
			QMessageBox::warning(parent, "Failed to Save Quiz", "The author needs to be set before saving.");
			return;
		}

		/** Get Quiz Description */
		const string quizDescription = data.quizDescription.toStdString();
		if ( quizName.empty() ) {
			QMessageBox::warning(parent, "Failed to Save Quiz", "The description needs to be set before saving.");
			return;
		}

		/** Check if quiz already exists */
		error_code filesystem_error;
		const string quizPath = config.getQuizDataPath() + "/" + quizName;
		if ( filesystem::is_directory(quizPath) ) {
			QMessageBox::StandardButton resBtn = QMessageBox::question(parent, "Overwrite Quiz?", "Quiz already exists, do you want to overwrite existing quiz?",
				QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

			if ( resBtn != QMessageBox::Yes ) {
				return;
			}
		} else if ( !filesystem::is_directory(quizPath) ) {
			/** Create Folder */
			filesystem::create_directory(quizPath, filesystem_error);
			if ( filesystem_error ) {
				QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to create directory to save the quiz in.");
				return;
			}
		}

		/** Create Media Folder */
		mediaDirectoryPath = quizPath + "/mediaTmp";
		filesystem::create_directory(mediaDirectoryPath, filesystem_error);
		if ( filesystem_error ) {
			QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to create directory to save the media files in.");
			return;
		}

		/** Wirte Quiz to XML Parser */
		boost::property_tree::ptree tree;
		boost::property_tree::ptree& main_tree = tree.put("MusicQuiz", "");
		main_tree.put("<xmlcomment>", string("File content written on the ") + common::TimeUtil::getTimeNow());

		/** Quiz Name */
		main_tree.put("QuizName", quizName);

		/** Quiz Author */
		main_tree.put("QuizAuthor", quizAuthor);

		/** Quiz Description */
		main_tree.put("QuizDescription", quizDescription);

		/** Guess the Category Setting */
		boost::property_tree::ptree& guessTheCategory_tree = main_tree.add("QuizGuessTheCategory", 500);
		guessTheCategory_tree.put<bool>("<xmlattr>.enabled", data.guessTheCategory);

		/** Categories */
		const size_t numberOfCategories = data.quizCategories.size();
		for ( size_t i = 0; i < numberOfCategories; ++i ) {
			/** Get Category */
			const MusicQuiz::CategoryCreator* category = data.quizCategories[i];

			/** Create Category tree */
			boost::property_tree::ptree& category_tree = main_tree.add("QuizCategories.Category", "");

			/** Category Name */
			const string categoryName = category->getName().toStdString();
			if ( categoryName.empty() ) {
				QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to save quiz. All categories must have a name.");
				deleteDirectory(mediaDirectoryPath);
				return;
			}

			/** Check if two categories have the same name */
			for ( size_t j = 0; j < numberOfCategories; ++j ) {
				if ( j != i ) {
					if ( categoryName == data.quizCategories[j]->getName().toStdString() ) {
						QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to save quiz. All categories must have a unique name.");
						deleteDirectory(mediaDirectoryPath);
						return;
					}
				}
			}
			category_tree.put("<xmlattr>.name", categoryName);

			/** Create Category Folder */
			filesystem::create_directory(mediaDirectoryPath + "/" + categoryName, filesystem_error);
			if ( filesystem_error ) {
				QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to create directory to save the catrgory files in.");
				deleteDirectory(mediaDirectoryPath);
				return;
			}

			/** Category Entries */
			const vector< MusicQuiz::EntryCreator* > entries = category->getEntries();
			const size_t numberOfEntries = entries.size();
			for ( size_t j = 0; j < numberOfEntries; ++j ) {
				/** Get Entry */
				MusicQuiz::EntryCreator* entry = entries[j];

				/** Create Entry tree */
				boost::property_tree::ptree& entry_tree = category_tree.add("QuizEntry", "");

				/** Entry Name */
				const string entryName = entry->getName().toStdString();
				if ( entryName.empty() ) {
					QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to save quiz. " + QString::fromStdString(categoryName) + ": All entries needs to have a name.");
					deleteDirectory(mediaDirectoryPath);
					return;
				}
				entry_tree.put("Answer", entryName);

				/** Check if two entries have the same name */
				for ( size_t k = 0; k < numberOfEntries; ++k ) {
					if ( k != j ) {
						if ( entryName == category->getEntries()[k]->getName().toStdString() ) {
							QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to save quiz. " + QString::fromStdString(categoryName) + ": All entires in a category must have a unique name.");
							deleteDirectory(mediaDirectoryPath);
							return;
						}
					}
				}
				entry_tree.put("<xmlattr>.name", entryName);

				/** Entry Type */
				const MusicQuiz::EntryCreator::EntryType type = entry->getType();
				if ( type == MusicQuiz::EntryCreator::EntryType::Song ) {
					entry_tree.put("<xmlattr>.type", "song");
				} else if ( type == MusicQuiz::EntryCreator::EntryType::Video ) {
					entry_tree.put("<xmlattr>.type", "video");
				}

				/** Entry Points */
				entry_tree.put("Points", entry->getPoints());

				/** Media */
				if ( type == MusicQuiz::EntryCreator::EntryType::Song ) { // Song
					/** Entry Song Start Time */
					entry_tree.put("StartTime", entry->getSongStartTime());

					/** Entry Song Answer Start Time */
					entry_tree.put("AnswerStartTime", entry->getAnswerStartTime());

					/** Media File */
					const string songFile = entry->getSongFile().toStdString();
					if ( !songFile.empty() ) {
						const string audioFileExtension = filesystem::path(songFile).extension().string();
						const string songPath = quizPath + "/media/" + categoryName + "/" + entryName + audioFileExtension;
						boost::property_tree::ptree& media_tree = entry_tree.add("Media", "");
						media_tree.put("SongFile", filesystem::relative(songPath, config.getQuizDataPath()).string());

						/** Copy Media File */
						filesystem::copy_file(songFile, mediaDirectoryPath + "/" + categoryName + "/" + entryName + audioFileExtension, filesystem::copy_options::overwrite_existing);
					}
				} else if ( type == MusicQuiz::EntryCreator::EntryType::Video ) { // Video
					/** Entry Video Start Time */
					entry_tree.put("StartTime", entry->getVideoStartTime());

					/** Entry Video Song Start Time */
					entry_tree.put("VideoSongStartTime", entry->getVideoSongStartTime());

					/** Entry Video Answer Start Time */
					entry_tree.put("AnswerStartTime", entry->getVideoAnswerStartTime());

					/** Media File */
					const string videoFile = entry->getVideoFile().toStdString();
					const string songFile = entry->getVideoSongFile().toStdString();

					if ( !videoFile.empty() && !songFile.empty() ) {
						const string videoFileExtension = filesystem::path(videoFile).extension().string();
						const string audioFileExtension = filesystem::path(songFile).extension().string();
						const string videoPath = quizPath + "/media/" + categoryName + "/" + entryName + "_video" + videoFileExtension;
						const string songPath = quizPath + "/media/" + categoryName + "/" + entryName + "_song" + audioFileExtension;
						boost::property_tree::ptree& media_tree = entry_tree.add("Media", "");
						media_tree.put("VideoFile", filesystem::relative(videoPath, config.getQuizDataPath()).string());
						media_tree.put("SongFile", filesystem::relative(songPath, config.getQuizDataPath()).string());

						/** Copy Media File */
						filesystem::copy_file(videoFile, mediaDirectoryPath + "/" + categoryName + "/" + entryName + "_video" + videoFileExtension, filesystem::copy_options::overwrite_existing);
						filesystem::copy_file(songFile, mediaDirectoryPath + "/" + categoryName + "/" + entryName + "_song" + audioFileExtension, filesystem::copy_options::overwrite_existing);
					}
				}
			}
		}

		/** Row Categories */
		const size_t numberOfRowCategories = data.quizRowCategories.size();
		for ( size_t i = 0; i < numberOfRowCategories; ++i ) {
			main_tree.add("QuizRowCategories.RowCategory", data.quizRowCategories[i].toStdString());
		}

		/** Delete Previous Folder */
		const filesystem::path target(quizPath + "/media");
		deleteDirectory(target);

		/** Rename tmp folder */
		filesystem::rename(mediaDirectoryPath, target);

		/** Save Quiz */
#if ( BOOST_VERSION >= 105600 )
		boost::property_tree::xml_writer_settings<string> settings('\t', 1);
#elif
		boost::property_tree::xml_writer_settings<char> settings('\t', 1);
#endif
		boost::property_tree::write_xml(quizPath + "/" + quizName + ".quiz.xml", tree, locale(), settings);

		/** Create Cheat Sheet */
		ofstream cheatSheet(quizPath + "/" + quizName + ".cheatsheet.txt");
		if ( cheatSheet.is_open() ) {
			cheatSheet << "--------------   CHEATSHEET   --------------\n"
				<< "Quiz: " << quizName << "\n"
				<< "Guess the Category: " << (data.guessTheCategory ? "Enabled" : "Disabled");

			const vector< MusicQuiz::CategoryCreator* > categories = data.quizCategories;
			for ( size_t i = 0; i < categories.size(); ++i ) {
				cheatSheet << "\n\n-----  " << categories[i]->getName().toStdString() << "  -----";
				vector< MusicQuiz::EntryCreator* > categoryEntries = data.quizCategories[i]->getEntries();
				for ( size_t j = 0; j < categoryEntries.size(); ++j ) {
					cheatSheet << "\n#" << j + 1 << " - " << categoryEntries[j]->getPoints() << " - " << categoryEntries[j]->getName().toStdString();
				}
			}
			cheatSheet.flush();
			cheatSheet.close();
		}

		/** Popup to tell user that the quiz was saved */
		QMessageBox::information(parent, "Info", "Quiz saves successfully.");
	} catch ( const exception& err ) {
		QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to save the quiz. " + QString::fromStdString(err.what()));
		deleteDirectory(mediaDirectoryPath);
	} catch ( ... ) {
		QMessageBox::warning(parent, "Failed to Save Quiz", "Failed to save the quiz. Unkown Error.");
		deleteDirectory(mediaDirectoryPath);
	}
}

MusicQuiz::QuizCreator::QuizData MusicQuiz::QuizFactory::loadQuiz(const string& quizName, const media::AudioPlayer::Ptr& audioPlayer,
	const common::Configuration& config, QWidget* parent)
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

	/** Quiz Data */
	MusicQuiz::QuizCreator::QuizData data;

	/** Load Categories */
	boost::property_tree::ptree tree;
	boost::property_tree::read_xml(quizList[idx], tree, boost::property_tree::xml_parser::trim_whitespace);
	boost::property_tree::ptree sub_tree = tree.get_child("MusicQuiz");

	/** Quiz Name */
	boost::property_tree::ptree::const_iterator ini_ctrl = tree.begin();
	data.quizName = QString::fromStdString(ini_ctrl->second.get<string>("QuizName"));

	/** Quiz Author */
	data.quizAuthor = QString::fromStdString(ini_ctrl->second.get<string>("QuizAuthor"));

	/** Quiz Description */
	data.quizDescription = QString::fromStdString(ini_ctrl->second.get<string>("QuizDescription"));

	/** Hidden Categories */
	data.guessTheCategory = ini_ctrl->second.get("QuizGuessTheCategory.<xmlattr>.enabled", false);

	/** Categories */
	vector< MusicQuiz::CategoryCreator* > categories;
	boost::property_tree::ptree::const_iterator ctrl = sub_tree.begin();
	for ( ; ctrl != sub_tree.end(); ++ctrl ) {
		if ( ctrl->first == "QuizCategories" ) {
			boost::property_tree::ptree categoriesTree = ctrl->second;
			boost::property_tree::ptree::const_iterator sub_ctrl = categoriesTree.begin();
			try {
				for ( ; sub_ctrl != categoriesTree.end(); ++sub_ctrl ) {
					if ( sub_ctrl->first == "Category" ) {
						/** Category Name */
						const QString categoryName = QString::fromStdString(sub_ctrl->second.get<string>("<xmlattr>.name"));

						/** Category */
						MusicQuiz::CategoryCreator* category = new MusicQuiz::CategoryCreator(categoryName, audioPlayer, config, parent);

						/** Category Entries */
						vector< MusicQuiz::EntryCreator* > categorieEntries;
						boost::property_tree::ptree entryTree = sub_ctrl->second;
						boost::property_tree::ptree::const_iterator it = entryTree.begin();
						for ( ; it != entryTree.end(); ++it ) {
							try {
								if ( it->first == "QuizEntry" ) {
									/** Settings */
									const QString entryName = QString::fromStdString(it->second.get<string>("<xmlattr>.name"));
									const int points = it->second.get<int>("Points");

									/** Quiz Entry */
									MusicQuiz::EntryCreator* entry = new MusicQuiz::EntryCreator(entryName, points, audioPlayer, config, category);

									/** Media Type */
									const string type = it->second.get<string>("<xmlattr>.type");
									if ( type == "song" ) { // Song
										entry->setType(MusicQuiz::EntryCreator::EntryType::Song);

										/** Start Time */
										try {
											entry->setSongStartTime(it->second.get<size_t>("StartTime"));
										} catch ( ... ) {}

										try {
											entry->setAnswerStartTime(it->second.get<size_t>("AnswerStartTime"));
										} catch ( ... ) {}

										/** Song File */
										try {
											QString songFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get<string>("Media.SongFile")));
											replace(songFile.begin(), songFile.end(), '\\', '/');
											entry->setSongFile(songFile);
										} catch ( ... ) {}
									} else if ( type == "video" ) { // Video
										entry->setType(MusicQuiz::EntryCreator::EntryType::Video);

										/** Song File */
										try {
											QString songFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get<string>("Media.SongFile")));
											replace(songFile.begin(), songFile.end(), '\\', '/');
											entry->setVideoSongFile(songFile);
										} catch ( ... ) {}

										try {
											entry->setVideoSongStartTime(it->second.get<size_t>("VideoSongStartTime"));
										} catch ( ... ) {}

										/** Video File */
										try {
											QString videoFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get<string>("Media.VideoFile")));
											replace(videoFile.begin(), videoFile.end(), '\\', '/');
											entry->setVideoFile(videoFile);
										} catch ( ... ) {}

										try {
											entry->setVideoStartTime(it->second.get<size_t>("StartTime"));
										} catch ( ... ) {}

										try {
											entry->setVideoAnswerStartTime(it->second.get<size_t>("AnswerStartTime"));
										} catch ( ... ) {}
									}
									categorieEntries.push_back(entry);
								}
							} catch ( ... ) {}
						}
						category->setEntries(categorieEntries);
						categories.push_back(category);
					}
				}
			} catch ( const exception& err ) {
				LOG_ERROR("Failed to load quiz. " << err.what());
			} catch ( ... ) {
				LOG_ERROR("Failed to load quiz.");
			}
		}
	}
	data.quizCategories = categories;

	/** Load Row Categories */
	data.quizRowCategories = MusicQuiz::util::QuizLoader::loadQuizRowCategories(idx, config);

	/** Return */
	return data;
}

void MusicQuiz::QuizFactory::deleteDirectory(const filesystem::path& dir)
{
	if ( filesystem::exists(dir) || filesystem::is_directory(dir) ) {
		filesystem::directory_iterator file(dir), end;
		for ( ; file != end; ++file ) {
			filesystem::remove_all(file->path());
		}
		filesystem::remove_all(dir);
	}
}