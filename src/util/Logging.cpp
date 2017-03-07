#include "Logging.hpp"

#include <plog/Formatters/FuncMessageFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

namespace gv
{
static bool gs_LoggingInitialized = false;

void InitializeConsoleOnlyLogging()
{
	static plog::ColorConsoleAppender<plog::FuncMessageFormatter> s_ConsoleLogAppender;

	if (!gs_LoggingInitialized)
	{
		plog::init(plog::debug, &s_ConsoleLogAppender);
		gs_LoggingInitialized = true;
	}
}

// Feel free to pass in NULL to get the default log file
void InitializeFileOnlyLogging(const char* filename)
{
	if (!gs_LoggingInitialized)
	{
		plog::init(plog::debug, filename ? filename : "LOCAL_Galavant.log");
		gs_LoggingInitialized = true;
	}
}
}