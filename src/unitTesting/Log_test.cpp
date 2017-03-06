#include <sys/stat.h>

#define CATCH_CONFIG_MAIN
#include "../../thirdParty/Catch/single_include/catch.hpp"

#include <plog/Log.h>

// for MyAppender
#include <plog/Formatters/FuncMessageFormatter.h>
#include <list>

// Copied from thirdParty/plog/samples/CustomAppender/Main.cpp
namespace plog
{
template <class Formatter>           // Typically a formatter is passed as a template parameter.
class MyAppender : public IAppender  // All appenders MUST inherit IAppender interface.
{
public:
	virtual void write(
	    const Record& record)  // This is a method from IAppender that MUST be implemented.
	{
		util::nstring str =
		    Formatter::format(record);  // Use the formatter to get a string from a record.

		m_messageList.push_back(str);  // Store a log message in a list.
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
}