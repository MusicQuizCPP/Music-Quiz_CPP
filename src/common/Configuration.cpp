#include "Configuration.hpp"

#include <filesystem>
#include <regex>

#include "common/Log.hpp"
#include "common/TimeUtil.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace common;
using namespace std;

Configuration::Configuration() : _quizDataPath("./data")
{}

string Configuration::getQuizDataPath() const
{
    return _quizDataPath;
}

void Configuration::setQuizDataPath(string path)
{
	/** Set the path */
    _quizDataPath = path;

	/** Update the config file */
	saveConfigurationFile();
}

bool Configuration::doQuizDataPathExist() const
{
    return filesystem::is_directory(_quizDataPath);
}

bool Configuration::doQuizConfigFileExist() const
{
	return filesystem::exists(_configFilePath);
}

void Configuration::loadConfigurationFile() 
{
	/** Sanity Check */
	if (!std::filesystem::exists(_configFilePath)) {
		throw std::runtime_error("The quiz config file does not exists.");
	}

	/** Load file */
	boost::property_tree::ptree tree;
	boost::property_tree::read_xml(_configFilePath, tree, boost::property_tree::xml_parser::trim_whitespace);
	
	/** Get quiz data path */
	boost::property_tree::ptree sub_tree = tree.get_child("Configuration");
	_quizDataPath = sub_tree.get< std::string >("DataPath");
}

void Configuration::saveConfigurationFile()
{
	/** Sanity Check */
	if (!doQuizDataPathExist()) {
		LOG_DEBUG("Invalid data path set, config file will not be created.");
		return;
	}

	/** Create Tree */
	boost::property_tree::ptree tree;
	boost::property_tree::ptree& main_tree = tree.put("Configuration", "");
	main_tree.put("<xmlcomment>", std::string("File content written on the ") + common::TimeUtil::getTimeNow());

	/** Set quiz data path */
	main_tree.put("DataPath", _quizDataPath);

	/** Save file */
	boost::property_tree::xml_writer_settings< std::string > settings('\t', 1);
	boost::property_tree::write_xml(_configFilePath, tree, std::locale(), settings);
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