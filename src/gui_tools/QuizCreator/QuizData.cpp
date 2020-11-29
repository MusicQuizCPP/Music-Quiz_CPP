#include "QuizData.hpp"

#include <exception>
#include <filesystem>
#include <system_error>
#include <fstream>
#include <algorithm>

#include <QTemporaryDir> 

#include <boost/property_tree/xml_parser.hpp>

#include "common/TimeUtil.hpp"
#include "common/Log.hpp"

#include "util/QuizLoader.hpp"

#include "CategoryCreator.hpp"
#include "EntryCreator.hpp"


using namespace std;

static void deleteDirectory(const filesystem::path& dir);

MusicQuiz::QuizData::QuizData(const common::Configuration& config, const string& name, const media::AudioPlayer::Ptr& audioPlayer, 
     QWidget* parent, bool skipEntries, regex categoryNameRegex) :
    _config(config),
    _name(name)
{

    /** Get List of Quizzes */
    vector<string> quizList = MusicQuiz::util::QuizLoader::getListOfQuizzes(config);
    if ( quizList.empty() ) {
        throw runtime_error("No quizzes found in the data folder.");
    }

    for ( auto& quizName : quizList) {
        replace(quizName.begin(), quizName.end(), '\\', '/');
    }

    /** Check if Quiz Exists */
    std::vector<string>::iterator quiz = std::find(quizList.begin(), quizList.end(), _name);
    if(quiz == quizList.end()) {
        throw runtime_error("Quiz does not exists.");
    }

    /** Load Categories */
    boost::property_tree::ptree tree;
    boost::property_tree::read_xml(*quiz, tree, boost::property_tree::xml_parser::trim_whitespace);
    boost::property_tree::ptree::const_iterator ini_ctrl = tree.begin();

    setName((ini_ctrl->second.get<string>("QuizName")));

    setAuthor(ini_ctrl->second.get<string>("QuizAuthor"));

    setDescription(ini_ctrl->second.get<string>("QuizDescription"));

    setGuessTheCategory(ini_ctrl->second.get("QuizGuessTheCategory.<xmlattr>.enabled", false), 500);

    setCategories(loadCategories(tree.get_child("MusicQuiz"), audioPlayer, skipEntries, categoryNameRegex, parent));

    setRowCategories(MusicQuiz::util::QuizLoader::loadQuizRowCategories(*quiz));

}

vector< MusicQuiz::CategoryCreator* > MusicQuiz::QuizData::loadCategories(boost::property_tree::ptree &tree, const media::AudioPlayer::Ptr& audioPlayer, bool skipEntries, regex categoryNameRegex, QWidget* parent) const
{
    (void)categoryNameRegex;

    vector< MusicQuiz::CategoryCreator* > categories;
    boost::property_tree::ptree::const_iterator ctrl = tree.begin();
    for ( ; ctrl != tree.end(); ++ctrl ) {
        if ( ctrl->first == "QuizCategories" ) {
            boost::property_tree::ptree categoriesTree = ctrl->second;
            boost::property_tree::ptree::const_iterator sub_ctrl = categoriesTree.begin();
            try {
                for ( ; sub_ctrl != categoriesTree.end(); ++sub_ctrl ) {
                    if ( sub_ctrl->first == "Category" ) {
                        const boost::property_tree::ptree &category_tree = sub_ctrl->second;
                        string name = category_tree.get<std::string>("<xmlattr>.name");
                        if(std::regex_match(name, categoryNameRegex)) {
                            categories.push_back(new MusicQuiz::CategoryCreator(category_tree, audioPlayer, _config, skipEntries, parent));
                        }
                    }
                }
            } catch ( const exception& err ) {
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
    for(size_t i = 0; i < _categories.size(); ++i) {
        for ( size_t j = 0; j < _categories.size(); ++j ) {
            if ( j != i && _categories[i]->getName().toStdString() == _categories[j]->getName().toStdString()) {
                return false;
            }
        }
    }
    return true;
}

void MusicQuiz::QuizData::save() const
{
    if (_name.empty()) {
        throw runtime_error("The quiz name needs to be set before saving.");
    } else if(_author.empty()) {
        throw runtime_error("The author needs to be set before saving.");
    } else if(_description.empty()) {
        throw runtime_error("The description needs to be set before saving.");
    }
    
    createQuizDirectory();
    QTemporaryDir tmpMediaDir(QString::fromStdString(getQuizPath() + "/tmpXXXXXX"));
    if(!tmpMediaDir.isValid()) {
        throw runtime_error("Failed to create directory to save the media files in.");
    }

    boost::property_tree::ptree tree = constructPtree(tmpMediaDir.path().toStdString());

    //Move media from tmp path to final path
    deleteDirectory(getMediaPath());
    filesystem::rename(tmpMediaDir.path().toStdString(), getMediaPath());

    //Save ptree to XML
    boost::property_tree::xml_writer_settings<string> settings('\t', 1);
    boost::property_tree::write_xml(getQuizPath() + "/" + _name + ".quiz.xml", tree, locale(), settings);

    saveCheatSheet(getQuizPath() + "/" + _name + ".cheatsheet.txt");
}

boost::property_tree::ptree MusicQuiz::QuizData::constructPtree(const string& savePath) const
{
    boost::property_tree::ptree tree;
    boost::property_tree::ptree& main_tree = tree.put("MusicQuiz", "");
    main_tree.put("<xmlcomment>", string("File content written on the ") + common::TimeUtil::getTimeNow());

    /** Quiz Name */
    main_tree.put("QuizName", _name);

    /** Quiz Author */
    main_tree.put("QuizAuthor", _author);

    /** Quiz Description */
    main_tree.put("QuizDescription", _description);

    /** Guess the Category Setting */
    boost::property_tree::ptree& guessTheCategory_tree = main_tree.add("QuizGuessTheCategory", 500);
    guessTheCategory_tree.put<bool>("<xmlattr>.enabled", _guessTheCategory);

    if(!areCategoryNamesUnique()) {
        throw runtime_error("Failed to save quiz. All categories must have an unique name");
    }

    for ( auto& category : _categories) {
        main_tree.add_child("QuizCategories.Category", category->saveToXml(savePath, getMediaPath()));
    }

    for ( auto& rowCategory : _rowCategories ) {
        main_tree.add("QuizRowCategories.RowCategory", rowCategory);
    }

    return tree;
}

void MusicQuiz::QuizData::saveCheatSheet(const string &path) const
{
    ofstream cheatSheet(path);
    if ( cheatSheet.is_open() ) {
        cheatSheet << "--------------   CHEATSHEET   --------------\n"
            << "Quiz: " << _name << "\n"
            << "Guess the Category: " << (_guessTheCategory ? "Enabled" : "Disabled");

        for ( auto category : _categories ) {
            cheatSheet << "\n\n-----  " << category->getName().toStdString() << "  -----";
            int i = 1;
            for ( auto entry : category->getEntries()) {
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

void MusicQuiz::QuizData::createDirectory(const string& path, const string& errorString) const
{
    if (!filesystem::is_directory(path)) {
        error_code error;
        filesystem::create_directory(path, error);
        if(error) {
            throw runtime_error(errorString);
        }
    }
}

string MusicQuiz::QuizData::getQuizPath() const
{
     return _config.getQuizDataPath() + "/" + _name;
}

string MusicQuiz::QuizData::getMediaPath() const
{
     return getQuizPath() + "/media";
}

bool MusicQuiz::QuizData::doesQuizDirectoryExist() const
{
    return filesystem::is_directory(getQuizPath());
}

void MusicQuiz::QuizData::setRowCategories(vector < string > rowCategories)
{
    _rowCategories = rowCategories;
}

void MusicQuiz::QuizData::setRowCategories(vector < QString > rowCategories)
{
    vector < string > stringVec;
    for(auto rowQString : rowCategories) {
        stringVec.push_back(rowQString.toStdString());
    }
    setRowCategories(stringVec);
}

MusicQuiz::CategoryCreator* MusicQuiz::QuizData::getCategory(const string& categoryName) const
{
    for(auto &category : _categories) {
        if(category->getName().toStdString() == categoryName) {
            return category;
        }
    }
    return nullptr;
}

void deleteDirectory(const filesystem::path& dir)
{
    if ( dir != "" && (filesystem::exists(dir) || filesystem::is_directory(dir)) ) {
        filesystem::directory_iterator file(dir), end;
        for ( ; file != end; ++file ) {
            filesystem::remove_all(file->path());
        }
        filesystem::remove_all(dir);
    }
}
