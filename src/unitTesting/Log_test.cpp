#include <sys/stat.h>

#define CATCH_CONFIG_MAIN
#include "../../thirdParty/Catch/single_include/catch.hpp"

#include <plog/Log.h>
#include <plog/Formatters/FuncMessageFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <list>

#include "../util/Logging.hpp"

// Copied from thirdParty/plog/samples/CustomAppender/Main.cpp
namespace plog
{
template <class Formatter>
class MyAppender : public IAppender
{
public:
	virtual void write(const Record& record)
	{
		util::nstring str = Formatter::format(record);

		m_messageList.push_back(str);
	}

	std::list<util::nstring>& getMessageList()
	{
		return m_messageList;
	}

private:
	std::list<util::nstring> m_messageList;
};
}

TEST_CASE("Log")
{
	SECTION("Log Writing to file")
	{
		const char* logName = "LOCAL_TestLog.log";
		plog::init(plog::debug, logName);

		LOG_DEBUG << "Test log";

		// To confirm the log worked, check if the file exists
		// http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
		struct stat buffer;
		REQUIRE(stat(logName, &buffer) == 0);
	}

	SECTION("Log Custom Appender")
	{
		static plog::MyAppender<plog::FuncMessageFormatter> myAppender;
		plog::init(plog::debug, &myAppender);

		LOGD << "A debug message!";

		REQUIRE(!myAppender.getMessageList().empty());
	}

	SECTION("Log Console Appender")
	{
		static plog::ColorConsoleAppender<plog::FuncMessageFormatter> s_ConsoleLogAppender;
		plog::init(plog::debug, &s_ConsoleLogAppender);

		for (int i = 0; i < 10; i++)
			LOGD << "[" << i << "]";

		LOGD << "You should see 1 - 9 above";

		// I don't know how to test this
		REQUIRE(true);
	}
}