#include "QuizData.hpp"

#include <fstream>
#include <algorithm>
#include <exception>
#include <filesystem>
#include <system_error>

#include <QTemporaryDir> 

#include <boost/property_tree/xml_parser.hpp>

#include "common/Log.hpp"
#include "common/TimeUtil.hpp"

#include "util/QuizLoader.hpp"

#include "EntryCreator.hpp"
#include "CategoryCreator.hpp"


static void deleteDirectory(const std::filesystem::path& dir);

MusicQuiz::QuizData::QuizData(const common::Configuration& config, const std::string& name, const media::AudioPlayer::Ptr& audioPlayer,
    const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer, QWidget* parent, bool skipEntries, std::regex categoryNameRegex) :
    _config(config), _name(name)
{
    /** Get List of Quizzes */
    std::vector< std::string > quizList = MusicQuiz::util::QuizLoader::getListOfQuizzes(config);
    if ( quizList.empty() ) {
        throw std::runtime_error("No quizzes found in the data folder.");
    }

    for ( auto& quizName : quizList ) {
        replace(quizName.begin(), quizName.end(), '\\', '/');
    }

    /** Check if Quiz Exists */
    std::vector< std::string >::iterator quiz = std::find(quizList.begin(), quizList.end(), _name);
    if ( quiz == quizList.end() ) {
        throw std::runtime_error("Quiz does not exists.");
    }

    /** Load Categories */
    boost::property_tree::ptree tree;
    boost::property_tree::read_xml(*quiz, tree, boost::property_tree::xml_parser::trim_whitespace);
    boost::property_tree::ptree::const_iterator ini_ctrl = tree.begin();

    setName((ini_ctrl->second.get< std::string >("QuizName")));

    setAuthor(ini_ctrl->second.get< std::string >("QuizAuthor"));

    setShowEntryTypeIcons(ini_ctrl->second.get("QuizShowEntryTypeIcons", false));

    const bool guessTheCategoryEnabled = ini_ctrl->second.get("QuizGuessTheCategory.<xmlattr>.enabled", false);
    const int guessTheCategoryPoints = ini_ctrl->second.get("QuizGuessTheCategory", 500);
    setGuessTheCategory(guessTheCategoryEnabled, guessTheCategoryPoints);

    setCategories(loadCategories(tree.get_child("MusicQuiz"), audioPlayer, textToSpeechPlayer, skipEntries, categoryNameRegex, parent));

    setRowCategories(MusicQuiz::util::QuizLoader::loadQuizRowCategories(*quiz));
}

std::vector< MusicQuiz::CategoryCreator* > MusicQuiz::QuizData::loadCategories(boost::property_tree::ptree &tree, const media::AudioPlayer::Ptr& audioPlayer,
    const media::TextToSpeechPlayer::Ptr& textToSpeechPlayer, bool skipEntries, std::regex categoryNameRegex, QWidget* parent) const
{
    (void)categoryNameRegex;

    std::vector< MusicQuiz::CategoryCreator* > categories;
    boost::property_tree::ptree::const_iterator ctrl = tree.begin();
    for ( ; ctrl != tree.end(); ++ctrl ) {
        if ( ctrl->first == "QuizCategories" ) {
            boost::property_tree::ptree categoriesTree = ctrl->second;
            boost::property_tree::ptree::const_iterator sub_ctrl = categoriesTree.begin();
            try {
                for ( ; sub_ctrl != categoriesTree.end(); ++sub_ctrl ) {
                    if ( sub_ctrl->first == "Category" ) {
                        const boost::property_tree::ptree &category_tree = sub_ctrl->second;
                        std::string name = category_tree.get<std::string>("<xmlattr>.name");
                        if ( std::regex_match(name, categoryNameRegex) ) {
                            categories.push_back(new MusicQuiz::CategoryCreator(category_tree, audioPlayer, textToSpeechPlayer, _config, skipEntries, parent));
                        }
                    }
                }
            } catch ( const std::exception& err ) {
                LOG_ERROR("Failed to load quiz. " << err.what());
            } catch ( ... ) {
                LOG_ERROR("Failed to load quiz.");
            }
        }
    }

    return categories;
}

bool MusicQuiz::QuizData::areCategoryNamesUnique() const 
{
    for ( size_t i = 0; i < _categories.size(); ++i ) {
        for ( size_t j = 0; j < _categories.size(); ++j ) {
            if ( j != i && _categories[i]->getName().toStdString() == _categories[j]->getName().toStdString() ) {
                return false;
            }
        }
    }

    return true;
}

void MusicQuiz::QuizData::save() const
{
    /** Sanity Check */
    if ( _name.empty() ) {
        throw std::runtime_error("The quiz name needs to be set before saving.");
    }

    /** Create Quiz Directory */
    createQuizDirectory();

    /** Create tempory directory outside the quiz folder */
    const std::filesystem::path quizPath = getQuizPath();
    QTemporaryDir tmpMediaDir(QString::fromStdString(quizPath.parent_path().string() + "/tmp"));
    if ( !tmpMediaDir.isValid() ) {
        throw std::runtime_error("Failed to create temporary directory to save the media files in.");
    }

    /** Create property tree */
    const std::string tmpPath = tmpMediaDir.path().toStdString();
    boost::property_tree::ptree tree = constructPtree(tmpPath);

    /** Move media from tmp path to final path.Try rename first, fallback to recursive copy */
    const std::string finalMediaPath = getMediaPath();
    try {
        /** Remove existing media directory(best - effort) before move */
        try {
            deleteDirectory(finalMediaPath);
        } catch ( const std::exception& e ) {
            /** Log and continue; rename may still fail and be handled below */
            LOG_ERROR("Failed to delete existing media directory: " << e.what());
        }

        /** Try rename */
        try {
            std::filesystem::rename(tmpPath, finalMediaPath);
        } catch ( const std::filesystem::filesystem_error& e ) {
            LOG_ERROR("Rename failed: " << e.what() << " - attempting recursive copy");

            /** Fallback: create destination and copy all files from tmpPath */
            std::error_code ec;
            std::filesystem::create_directories(finalMediaPath, ec);
            if ( ec ) {
                throw std::runtime_error(std::string("Failed to create media directory: ") + ec.message());
            }

            for ( auto it = std::filesystem::recursive_directory_iterator(tmpPath); it != std::filesystem::recursive_directory_iterator(); ++it ) {
                const std::filesystem::path src = it->path();
                const std::filesystem::path relative = std::filesystem::relative(src, tmpPath);
                const std::filesystem::path dest = std::filesystem::path(finalMediaPath) / relative;

                if ( std::filesystem::is_directory(src) ) {
                    std::filesystem::create_directories(dest, ec);
                    if ( ec ) {
                        throw std::runtime_error(std::string("Failed to create directory '") + dest.string() + "': " + ec.message());
                    }
                } else {
                    std::filesystem::create_directories(dest.parent_path(), ec);
                    if ( ec ) {
                        throw std::runtime_error(std::string("Failed to create directory '") + dest.parent_path().string() + "': " + ec.message());
                    }
                    std::filesystem::copy_file(src, dest, std::filesystem::copy_options::overwrite_existing, ec);
                    if ( ec ) {
                        throw std::runtime_error(std::string("Failed to copy file '") + src.string() + "' -> '" + dest.string() + "': " + ec.message());
                    }
                }
            }
        }
    } catch ( const std::exception& e ) {
        throw std::runtime_error(std::string("Failed to move media files: ") + e.what());
    }

    /** Save ptree to XML */
    try {
        boost::property_tree::xml_writer_settings< std::string > settings('\t', 1);
        boost::property_tree::write_xml(getQuizPath() + "/" + _name + ".quiz.xml", tree, std::locale(), settings);
    } catch ( const std::exception& e ) {
        throw std::runtime_error(std::string("Failed to write quiz xml: ") + e.what());
    }

    /** Save cheatsheet */
    saveCheatSheet(getQuizPath() + "/" + _name + ".cheatsheet.txt");
}

boost::property_tree::ptree MusicQuiz::QuizData::constructPtree(const std::string& savePath) const
{
    boost::property_tree::ptree tree;
    boost::property_tree::ptree& main_tree = tree.put("MusicQuiz", "");
    main_tree.put("<xmlcomment>", std::string("File content written on the ") + common::TimeUtil::getTimeNow());

    /** Quiz Name */
    main_tree.put("QuizName", _name);

    /** Quiz Author */
    main_tree.put("QuizAuthor", _author);

    /** Show Entry Type Icons Setting */
    main_tree.put("QuizShowEntryTypeIcons", _showEntryTypeIcons);

    /** Guess the Category Setting */
    boost::property_tree::ptree& guessTheCategory_tree = main_tree.add("QuizGuessTheCategory", _guessTheCategoryPoints);
    guessTheCategory_tree.put<bool>("<xmlattr>.enabled", _guessTheCategory);

    if ( !areCategoryNamesUnique() ) {
        throw std::runtime_error("Failed to save quiz. All categories must have an unique name");
    }

    for ( auto& category : _categories ) {
        main_tree.add_child("QuizCategories.Category", category->saveToXml(savePath, getMediaPath()));
    }

    for ( auto& rowCategory : _rowCategories ) {
        main_tree.add("QuizRowCategories.RowCategory", rowCategory);
    }

    return tree;
}

void MusicQuiz::QuizData::saveCheatSheet(const std::string &path) const
{
    std::ofstream cheatSheet(path);
    if ( cheatSheet.is_open() ) {
        cheatSheet << "--------------   CHEATSHEET   --------------\n"
            << "Quiz: " << _name << "\n"
            << "Guess the Category: " << (_guessTheCategory ? "Enabled" : "Disabled");

        for ( auto category : _categories ) {
            cheatSheet << "\n\n-----  " << category->getName().toStdString() << "  -----";
            int i = 1;
            for ( auto entry : category->getEntries() ) {
                cheatSheet << "\n#" << i++ << " - " << entry->getPoints() << " - " << entry->getName().toStdString();
            }
        }

        cheatSheet.close();
    }
}

void MusicQuiz::QuizData::createQuizDirectory() const
{
    createDirectory(getQuizPath(), "Failed to create directory to save the quiz in.");
}

void MusicQuiz::QuizData::createDirectory(const std::string& path, const std::string& errorString) const
{
    if ( !std::filesystem::is_directory(path) ) {
        std::error_code error;
        std::filesystem::create_directory(path, error);
        if ( error ) {
            throw std::runtime_error(errorString);
        }
    }
}

std::string MusicQuiz::QuizData::getQuizPath() const
{
     return _config.getQuizDataPath() + "/" + _name;
}

std::string MusicQuiz::QuizData::getMediaPath() const
{
     return getQuizPath() + "/media";
}

bool MusicQuiz::QuizData::doesQuizDirectoryExist() const
{
    return std::filesystem::is_directory(getQuizPath());
}

void MusicQuiz::QuizData::setRowCategories(std::vector< std::string > rowCategories)
{
    _rowCategories = rowCategories;
}

void MusicQuiz::QuizData::setRowCategories(std::vector< QString > rowCategories)
{
    std::vector< std::string > stringVec;
    for ( auto rowQString : rowCategories ) {
        stringVec.push_back(rowQString.toStdString());
    }

    setRowCategories(stringVec);
}

MusicQuiz::CategoryCreator* MusicQuiz::QuizData::getCategory(const std::string& categoryName) const
{
    for ( auto &category : _categories ) {
        if ( category->getName().toStdString() == categoryName ) {
            return category;
        }
    }

    return nullptr;
}

void deleteDirectory(const std::filesystem::path& dir)
{
    if ( dir != "" && (std::filesystem::exists(dir) || std::filesystem::is_directory(dir)) ) {
        std::filesystem::directory_iterator file(dir), end;
        for ( ; file != end; ++file ) {
            std::filesystem::remove_all(file->path());
        }

        std::filesystem::remove_all(dir);
    }
}