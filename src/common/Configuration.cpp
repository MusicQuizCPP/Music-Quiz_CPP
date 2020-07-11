#include "Configuration.hpp"

#include <filesystem>
#include <regex>

using namespace common;
using namespace std;

Configuration::Configuration() : _quizDataPath("./data")
{
}

string Configuration::getQuizDataPath() const
{
    return _quizDataPath;
}

void Configuration::setQuizDataPath(string path)
{
    _quizDataPath = path;
}

bool Configuration::doQuizDataPathExist() const
{
    return filesystem::is_directory(_quizDataPath);
}

string Configuration::mediaPathToFullPath(string mediaPathStr) const
{
	filesystem::path quizDataPath(getQuizDataPath());
	filesystem::path mediaPath(mediaPathStr);

	filesystem::path fullpath = quizDataPath / mediaPath;

	if( !filesystem::exists(fullpath)) //Hack for compatibility with older quizes
	{
		mediaPath = regex_replace(mediaPathStr, regex("\\./data/"), "");
		if(filesystem::exists(quizDataPath / mediaPath))
		{
			fullpath = quizDataPath / mediaPath;
		}
	}
	return fullpath.string();
}