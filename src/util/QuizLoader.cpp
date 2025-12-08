#include "QuizLoader.hpp"

#include <stdexcept>
#include <algorithm>
#include <filesystem>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <QVoice>

#include "common/Log.hpp"
#include "common/Configuration.hpp"

#include "gui_tools/widgets/QuizEntry.hpp"


std::vector< std::string > MusicQuiz::util::QuizLoader::getListOfQuizzes(const common::Configuration& config)
{
	/** Check if data folder exists */
	const std::filesystem::path dataFolder = config.getQuizDataPath();
	if ( !std::filesystem::is_directory(dataFolder) ) {
		throw std::runtime_error("Data folder does not exists.");
	}

	/** Find Quizzes */
	std::vector< std::string > quizList;
	std::filesystem::recursive_directory_iterator end;

	for ( std::filesystem::recursive_directory_iterator file(dataFolder); file != end; ++file ) {
		const std::string fileStr = std::filesystem::path(*file).string();

		/** Skip non quiz files */
		if ( fileStr.find(".quiz.xml") == std::string::npos ) {
			continue;
		}

		/** Push back quiz */
		quizList.push_back(fileStr);
	}

	return quizList;
}

MusicQuiz::util::QuizLoader::QuizPreview MusicQuiz::util::QuizLoader::getQuizPreview(size_t idx, const common::Configuration& config)
{
	/** Get List of Quizzes */
	const std::vector< std::string > quizList = getListOfQuizzes(config);
	if ( quizList.empty() ) {
		throw std::runtime_error("No quizzes found in the data folder.");
	}

	/** Sanity Check */
	if ( idx >= quizList.size() ) {
		throw std::runtime_error("Index out of range.");
	}

	if ( !std::filesystem::exists(quizList[idx]) ) {
		throw std::runtime_error("Quiz file does not exists.");
	}

	/** Load preview */
	QuizLoader::QuizPreview quizPreview;
	boost::property_tree::ptree tree;
	boost::property_tree::read_xml(quizList[idx], tree, boost::property_tree::xml_parser::trim_whitespace);
	boost::property_tree::ptree sub_tree = tree.get_child("MusicQuiz");

	/** Name */
	quizPreview.quizName = sub_tree.get< std::string >("QuizName");

	/** Author */
	quizPreview.quizAuthor = sub_tree.get< std::string >("QuizAuthor");

	/** Guess the Category */
	quizPreview.guessTheCategory = sub_tree.get("QuizGuessTheCategory.<xmlattr>.enabled", false);

	/** Categories & Row Categories */
	boost::property_tree::ptree::const_iterator ctrl = sub_tree.begin();
	for ( ; ctrl != sub_tree.end(); ++ctrl ) {
		if ( ctrl->first == "QuizCategories" ) { // Load Categories
			boost::property_tree::ptree categoriesTree = ctrl->second;
			boost::property_tree::ptree::const_iterator sub_ctrl = categoriesTree.begin();
			for ( ; sub_ctrl != categoriesTree.end(); ++sub_ctrl ) {
				if ( sub_ctrl->first == "Category" ) {
					quizPreview.categories.push_back(sub_ctrl->second.get< std::string >("<xmlattr>.name"));

					if ( !quizPreview.includeSongs || !quizPreview.includeVideos ) { // Check if the quiz contains songs / videos.
						boost::property_tree::ptree mediaTree = sub_ctrl->second;
						boost::property_tree::ptree::const_iterator it = mediaTree.begin();
						for ( ; it != mediaTree.end(); ++it ) {
							if ( it->first == "QuizEntry" ) {
								if ( it->second.get< std::string >("<xmlattr>.type") == "song" && !quizPreview.includeSongs ) {
									quizPreview.includeSongs = true;
								} else if ( it->second.get< std::string >("<xmlattr>.type") == "video" && !quizPreview.includeVideos ) {
									quizPreview.includeVideos = true;
								} else if ( it->second.get< std::string >("<xmlattr>.type") == "textToSpeech" && !quizPreview.includeVideos ) {
									quizPreview.includeTextToSpeech = true;
								} else if ( it->second.get< std::string >("<xmlattr>.type") == "image" && !quizPreview.includeImages ) {
									quizPreview.includeImages = true;
								}
							}
						}
					}
				}
			}
		} else if ( ctrl->first == "QuizRowCategories" ) { // Load Row Categories
			boost::property_tree::ptree rowCategoriesTree = ctrl->second;
			boost::property_tree::ptree::const_iterator sub_ctrl = rowCategoriesTree.begin();
			for ( ; sub_ctrl != rowCategoriesTree.end(); ++sub_ctrl ) {
				if ( sub_ctrl->first == "RowCategory" ) {
					quizPreview.rowCategories.push_back(sub_ctrl->second.data());
				}
			}
		}
	}

	return quizPreview;
}

std::vector< MusicQuiz::QuizCategory* > MusicQuiz::util::QuizLoader::loadQuizCategories(const size_t idx, const media::AudioPlayer::Ptr& audioPlayer,
	const media::VideoPlayer::Ptr& videoPlayer, const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer, const media::ImagePlayer::Ptr& imagePlayer, const common::Configuration& config, std::string& err)
{
	/** Get List of Quizzes */
	const std::vector< std::string > quizList = getListOfQuizzes(config);
	if ( quizList.empty() ) {
		throw std::runtime_error("No quizzes found in the data folder.");
	}

	/** Sanity Check */
	if ( idx >= quizList.size() ) {
		throw std::runtime_error("No quiz index requested does not exists.");
	}

	if ( !std::filesystem::exists(quizList[idx]) ) {
		throw std::runtime_error("Quiz file does not exists.");
	}


	/** Load Categories */
	LOG_INFO("Loading Quiz #" << idx << " '" << quizList[idx] << "'.");

	boost::property_tree::ptree tree;
	boost::property_tree::read_xml(quizList[idx], tree, boost::property_tree::xml_parser::trim_whitespace);
	boost::property_tree::ptree sub_tree = tree.get_child("MusicQuiz");

	std::vector< MusicQuiz::QuizCategory* > categories;
	boost::property_tree::ptree::const_iterator ctrl = sub_tree.begin();
	for ( ; ctrl != sub_tree.end(); ++ctrl ) {
		if ( ctrl->first == "QuizCategories" ) { // Load Categories
			boost::property_tree::ptree categoriesTree = ctrl->second;
			boost::property_tree::ptree::const_iterator sub_ctrl = categoriesTree.begin();
			try {
				for ( ; sub_ctrl != categoriesTree.end(); ++sub_ctrl ) {
					if ( sub_ctrl->first == "Category" ) {
						/** Category Name */
						const QString categoryName = QString::fromStdString(sub_ctrl->second.get< std::string >("<xmlattr>.name"));

						/** Category Entries */
						std::vector< MusicQuiz::QuizEntry* > categorieEntries;
						boost::property_tree::ptree entryTree = sub_ctrl->second;
						boost::property_tree::ptree::const_iterator it = entryTree.begin();
						for ( ; it != entryTree.end(); ++it ) {
							if ( it->first == "QuizEntry" ) {
								/** Settings */
								const QString answer = QString::fromStdString(it->second.get< std::string >("Answer"));
								const size_t points = it->second.get<size_t>("Points");

								/** Media Type */
								const std::string type = it->second.get< std::string >("<xmlattr>.type");
								if ( type == "song" ) { // Song
									QString songFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get< std::string >("Media.SongFile")));
									const size_t audioStartTime = it->second.get<size_t>("StartTime");
									const size_t answerStartTime = it->second.get<size_t>("AnswerStartTime");
									std::replace(songFile.begin(), songFile.end(), '\\', '/');

									/** Check if file exsists */
									if ( !std::filesystem::exists(songFile.toStdString()) ) {
										err += "Missing song file '" + songFile.toStdString() + "'\n";
									}

									/** Push Back Song Entry */
									categorieEntries.push_back(new MusicQuiz::QuizEntry(songFile, answer, points, audioStartTime, answerStartTime, audioPlayer, videoPlayer, textToSpeechPlayer, imagePlayer));
								} else if ( type == "video" ) { // Video
									QString songFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get< std::string >("Media.SongFile")));
									QString videoFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get< std::string >("Media.VideoFile")));
									std::replace(songFile.begin(), songFile.end(), '\\', '/');
									std::replace(videoFile.begin(), videoFile.end(), '\\', '/');
									const size_t videoStartTime = it->second.get<size_t>("StartTime");
									const size_t videoSongStartTime = it->second.get<size_t>("VideoSongStartTime");
									const size_t answerStartTime = it->second.get<size_t>("AnswerStartTime");

									/** Check if files exsists */
									if ( !std::filesystem::exists(songFile.toStdString()) ) {
										err += "Missing song file '" + songFile.toStdString() + "'\n";
									}

									if ( !std::filesystem::exists(videoFile.toStdString()) ) {
										err += "Missing video file '" + videoFile.toStdString() + "'\n";
									}

									/** Push Back Video Entry */
									categorieEntries.push_back(new MusicQuiz::QuizEntry(songFile, videoFile, answer, points, videoSongStartTime, videoStartTime, answerStartTime, audioPlayer, videoPlayer, textToSpeechPlayer, imagePlayer));
								} else if ( type == "textToSpeech" ) {// text to speech
									/** Get Speech String */
									const QString textToSpeechString = QString::fromStdString(it->second.get<std::string>("Media.TextToSpeechString"));

									/** Get Settings */
									media::TextToSpeechPlayer::TextToSpeechSettings settings;
									settings._pitch = it->second.get<double>("Media.Pitch", 0.0);
									settings._rate = it->second.get<double>("Media.Rate", 0.0);
									const QString voiceName = QString::fromStdString(it->second.get<std::string>("Media.VoiceName", ""));
									const QVector< QVoice > avaliableVoices = textToSpeechPlayer->availableVoices();
									for ( int i = 0; i < avaliableVoices.size(); ++i ) {
										if ( voiceName == avaliableVoices[i].name() ) {
											settings._voice = avaliableVoices[i];
										}
									}

									/** Check if string has been set */
									if ( textToSpeechString.isEmpty() ) {
										err += "Text to speech string is empty\n";
									}

									/** Get Answer Song File */
									QString songFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get< std::string >("Media.SongFile")));
									const size_t answerStartTime = it->second.get<size_t>("AnswerStartTime");
									std::replace(songFile.begin(), songFile.end(), '\\', '/');

									/** Check if file exsists */
									if (!std::filesystem::exists(songFile.toStdString())) {
										err += "Missing song file '" + songFile.toStdString() + "'\n";
									}

									/** Push Back Text to Speech Entry */
									categorieEntries.push_back(new MusicQuiz::QuizEntry(textToSpeechString, songFile, answer, points, answerStartTime, audioPlayer, videoPlayer, textToSpeechPlayer, settings, imagePlayer));
								} else if ( type == "image" ) { // image
									QString imageFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get< std::string >("Media.ImageFile")));
									QString songFile = QString::fromStdString(config.mediaPathToFullPath(it->second.get< std::string >("Media.SongFile")));
									std::replace(imageFile.begin(), imageFile.end(), '\\', '/');
									std::replace(songFile.begin(), songFile.end(), '\\', '/');
									const size_t answerStartTime = it->second.get<size_t>("AnswerStartTime");

									/** Check if files exsists */
									if ( !std::filesystem::exists(imageFile.toStdString()) ) {
										err += "Missing image file '" + imageFile.toStdString() + "'\n";
									}

									if ( !std::filesystem::exists(songFile.toStdString()) ) {
										err += "Missing song file '" + songFile.toStdString() + "'\n";
									}

									/** Push Back Image Entry */
									categorieEntries.push_back(new MusicQuiz::QuizEntry(imageFile, songFile, answer, points, answerStartTime, audioPlayer, videoPlayer, textToSpeechPlayer, imagePlayer));
								}
							}
						}

						categories.push_back(new MusicQuiz::QuizCategory(categoryName, categorieEntries));
					}
				}
			} catch ( const std::exception& error ) {
				LOG_ERROR("Failed to load category. " << error.what());
			} catch ( ... ) {
				LOG_ERROR("Failed to load category.");
			}
		}
	}

	return categories;
}

std::vector< QString > MusicQuiz::util::QuizLoader::loadQuizRowCategories(const size_t idx, const common::Configuration& config)
{
	/** Get List of Quizzes */
	const std::vector< std::string > quizList = getListOfQuizzes(config);

	if ( quizList.empty() ) {
		throw std::runtime_error("No quizzes found in the data folder.");
	}

	/** Sanity Check */
	if ( idx >= quizList.size() ) {
		throw std::runtime_error("The quiz index requested does not exists.");
	}

	/** Sanity Check */
	if ( idx >= quizList.size() ) {
		throw std::runtime_error("No quiz index requested does not exists.");
	}
	return loadQuizRowCategories(quizList[idx]);
}

std::vector< QString > MusicQuiz::util::QuizLoader::loadQuizRowCategories(const std::string& xmlPath)
{
	if ( !std::filesystem::exists(xmlPath) ) {
		throw std::runtime_error("Quiz file does not exists.");
	}


	/** Load Row Categories */
	boost::property_tree::ptree tree;
	boost::property_tree::read_xml(xmlPath, tree, boost::property_tree::xml_parser::trim_whitespace);
	boost::property_tree::ptree sub_tree = tree.get_child("MusicQuiz");

	std::vector< QString > rowCategories;
	boost::property_tree::ptree::const_iterator ctrl = sub_tree.begin();
	for ( ; ctrl != sub_tree.end(); ++ctrl ) {
		if ( ctrl->first == "QuizRowCategories" ) { // Load Categories
			boost::property_tree::ptree rowCategoriesTree = ctrl->second;
			boost::property_tree::ptree::const_iterator sub_ctrl = rowCategoriesTree.begin();
			for ( ; sub_ctrl != rowCategoriesTree.end(); ++sub_ctrl ) {
				if ( sub_ctrl->first == "RowCategory" ) {
					rowCategories.push_back(QString::fromStdString(sub_ctrl->second.data()));
				}
			}
		}
	}

	return rowCategories;
}