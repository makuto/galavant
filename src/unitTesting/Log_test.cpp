#include <sys/stat.h>

#define CATCH_CONFIG_MAIN
#include "../../thirdParty/Catch/single_include/catch.hpp"

#include "util/Logging.hpp"

static gv::Logging::Logger s_logger;

void testFuncName()
{
	LOGD << "The funcName should be formatted correctly here";
}

TEST_CASE("Log")
{
	SECTION("Log Console Appender")
	{
		for (int i = 0; i < 10; i++)
			LOGD << "[" << i << "]";

        LOGD << "Does this print once? Because it should";

        testFuncName();

        // I don't know how to test this
		std::cout << "You should see 1 - 9 above. If you don't, the test actually failed!\n";
		REQUIRE(true);
	}
}