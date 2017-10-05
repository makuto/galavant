#pragma once

// size_t
#include <stddef.h>

// This file is based on PLog's interface but uses C strings only. See Plog:
//  Plog - portable and simple log for C++
//  Documentation and sources: https://github.com/SergiusTheBest/plog
//  License: MPL 2.0, http://mozilla.org/MPL/2.0/
// I had to cut PLog out because Unreal doesn't allow the use of std::string in any code

//////////////////////////////////////////////////////////////////////////
// Helper macros that get context info

/*#ifdef _MSC_VER
#   define LOGGER_GET_FUNC()      __FUNCTION__
#elif defined(__BORLANDC__)
#   define LOGGER_GET_FUNC()      __FUNC__
#else
#   define LOGGER_GET_FUNC()      __PRETTY_FUNCTION__
#endif*/
#define LOGGER_GET_FUNC() __PRETTY_FUNCTION__

/*#if gv::Logger_CAPTURE_FILE
#   define LOGGER_GET_FILE()      __FILE__
#else
#   define LOGGER_GET_FILE()      ""
#endif*/
#define LOGGER_GET_FILE() __FILE__

//////////////////////////////////////////////////////////////////////////
// Log severity level checker

#define IF_LOG_(severity)                                             \
	!(gv::Logging::Logger::GetSingleton() &&                          \
	  gv::Logging::Logger::GetSingleton()->checkSeverity(severity)) ? \
	    (void)0:
#define IF_LOG(severity) IF_LOG_(severity)

//////////////////////////////////////////////////////////////////////////
// Main logging macros

#define LOG_(severity) \
	IF_LOG_(severity) (*gv::Logging::Logger::GetSingleton()) += gv::Logging::Record(severity, LOGGER_GET_FUNC(), __LINE__, LOGGER_GET_FILE())
#define LOG(severity) LOG_(severity)

#define LOG_VERBOSE LOG(gv::Logging::Severity::verbose)
#define LOG_DEBUG LOG(gv::Logging::Severity::debug)
#define LOG_INFO LOG(gv::Logging::Severity::info)
#define LOG_WARNING LOG(gv::Logging::Severity::warning)
#define LOG_ERROR LOG(gv::Logging::Severity::error)
#define LOG_FATAL LOG(gv::Logging::Severity::fatal)

#define LOGV LOG_VERBOSE
#define LOGD LOG_DEBUG
#define LOGI LOG_INFO
#define LOGW LOG_WARNING
#define LOGE LOG_ERROR
#define LOGF LOG_FATAL

//////////////////////////////////////////////////////////////////////////
// Conditional logging macros

#define LOG_IF_(severity, condition) !(condition) ? void(0) : LOG_(severity)
#define LOG_IF(severity, condition) LOG_IF_(severity, condition)

#define LOG_VERBOSE_IF(condition) LOG_IF(gv::Logging::Severity::verbose, condition)
#define LOG_DEBUG_IF(condition) LOG_IF(gv::Logging::Severity::debug, condition)
#define LOG_INFO_IF(condition) LOG_IF(gv::Logging::Severity::info, condition)
#define LOG_WARNING_IF(condition) LOG_IF(gv::Logging::Severity::warning, condition)
#define LOG_ERROR_IF(condition) LOG_IF(gv::Logging::Severity::error, condition)
#define LOG_FATAL_IF(condition) LOG_IF(gv::Logging::Severity::fatal, condition)

#define LOGV_IF(condition) LOG_VERBOSE_IF(condition)
#define LOGD_IF(condition) LOG_DEBUG_IF(condition)
#define LOGI_IF(condition) LOG_INFO_IF(condition)
#define LOGW_IF(condition) LOG_WARNING_IF(condition)
#define LOGE_IF(condition) LOG_ERROR_IF(condition)
#define LOGF_IF(condition) LOG_FATAL_IF(condition)

namespace gv
{
namespace Logging
{
enum class Severity
{
	none = 0,
	fatal,
	error,
	warning,
	info,
	debug,
	verbose
};

inline const char* severityToString(Severity severity)
{
	switch (severity)
	{
		case Severity::fatal:
			return "FATAL";
		case Severity::error:
			return "ERROR";
		case Severity::warning:
			return "WARN";
		case Severity::info:
			return "INFO";
		case Severity::debug:
			return "DEBUG";
		case Severity::verbose:
			return "VERB";
		default:
			return "NONE";
	}
}

inline Severity severityFromString(const char* str)
{
	for (Severity severity = Severity::fatal; severity <= Severity::verbose;
	     severity = static_cast<Severity>((int)severity + 1))
	{
		if (severityToString(severity)[0] == str[0])
		{
			return severity;
		}
	}

	return Severity::none;
}

struct Record
{
	Severity severity;
	const char* const Function;
	size_t Line;
	const char* const File;

#define OUT_BUFFER_SIZE 1024
	char OutBuffer[OUT_BUFFER_SIZE];
	size_t Offset;

	Record(Severity newSeverity, const char* func, size_t line, const char* file);
	Record& operator<<(char data);
	Record& operator<<(const char* data);
	Record& operator<<(const int data);
	Record& operator<<(const unsigned int data);
	Record& operator<<(const float data);
	Record& operator<<(const bool data);
	Record& operator<<(const size_t data);
	template <class T>
	Record& operator<<(const T& data)
	{
		return *this;
	}
};

using CustomLogOutputFunc = void (*)(const Record&);

void FormatFuncName(char* buffer, const char* func, size_t bufferSize);

class Logger
{
private:
	Severity MaxSeverity;
	static Logger* Singleton;
	CustomLogOutputFunc CustomOutputFunc;

public:
	Logger(Severity maxSeverity = Severity::verbose,
	       CustomLogOutputFunc customOutputFunc = nullptr);
	bool checkSeverity(Severity severity) const;

	void operator+=(const Record& record);

	static Logger* GetSingleton();
};
}
}