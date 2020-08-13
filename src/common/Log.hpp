#pragma once

#if defined(LOG_DEBUG)
#undef LOG_DEBUG
#endif

#if defined(LOG_INFO)
#undef LOG_INFO
#endif

#if defined(LOG_WARN)
#undef LOG_WARN
#endif

#if defined(LOG_ERROR)
#undef LOG_ERROR
#endif

#include <string>
#include <sstream>
#include <iostream>

#if defined(_WIN32) || defined(WIN32)
constexpr char FOLDER_SEPERATOR('\\');
#else
constexpr char FOLDER_SEPERATOR('/');
#endif

#ifndef NDEBUG
#define LOG_DEBUG(a) {\
	const std::string log_file_path = std::string(__FILE__); \
	const size_t  log_idx = log_file_path.find_last_of(FOLDER_SEPERATOR, log_file_path.length());  \
	std::stringstream log_common_stream; \
	log_common_stream << log_file_path.substr(log_idx + 1, std::string::npos) << "::" << __func__ << ":" << __LINE__ << ": " << a << "\n"; \
	std::cout << log_common_stream.str() << std::flush;}
#else
#define LOG_DEBUG(a)
#endif

#define LOG_INFO(a) {\
	const std::string log_file_path = std::string(__FILE__); \
	const size_t  log_idx = log_file_path.find_last_of(FOLDER_SEPERATOR, log_file_path.length());  \
	std::stringstream log_common_stream; \
	log_common_stream << log_file_path.substr(log_idx + 1, std::string::npos) << "::" << __func__ << ":" << __LINE__ << ": " << a << "\n"; \
	std::cout << log_common_stream.str() << std::flush;}

#define LOG_WARN(a) {\
	const std::string log_file_path = std::string(__FILE__); \
	const size_t  log_idx = log_file_path.find_last_of(FOLDER_SEPERATOR, log_file_path.length());  \
	std::stringstream log_common_stream; \
	log_common_stream << log_file_path.substr(log_idx + 1, std::string::npos) << "::" << __func__ << ":" << __LINE__ << ": " << a << "\n"; \
	std::cout << log_common_stream.str() << std::flush;}

#define LOG_ERROR(a) {\
	const std::string log_file_path = std::string(__FILE__); \
	const size_t  log_idx = log_file_path.find_last_of(FOLDER_SEPERATOR, log_file_path.length());  \
	std::stringstream log_common_stream; \
	log_common_stream << log_file_path.substr(log_idx + 1, std::string::npos) << "::" << __func__ << ":" << __LINE__ << ": " << a << "\n"; \
	std::cout << log_common_stream.str() << std::flush;}
