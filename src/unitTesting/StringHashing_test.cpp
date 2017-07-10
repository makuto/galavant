#pragma once

#include <iostream>

#define CATCH_CONFIG_MAIN
#include "../../thirdParty/Catch/single_include/catch.hpp"

#include "../util/StringHashing.hpp"

TEST_CASE("String Hashing")
{
	SECTION("Compile time string hashing matches runtime hashing")
	{
		const char* dynamicString = "This_Is_A_Test_String_For_Crc_Hashing";
		REQUIRE(COMPILE_TIME_CRC32_STR("This_Is_A_Test_String_For_Crc_Hashing") ==
		        stringCrc32(dynamicString));
	}

	SECTION("Compile time string hashing does not match other string")
	{
		REQUIRE(COMPILE_TIME_CRC32_STR("This_Is_A_Test_String_For_Crc_Hashing") !=
		        COMPILE_TIME_CRC32_STR("This_Is_A_Different_Test_String_For_Crc_Hashing"));
	}

	SECTION("Empty strings")
	{
		REQUIRE(!COMPILE_TIME_CRC32_STR(""));
		REQUIRE(!stringCrc32(""));
		REQUIRE(!stringCrc32(nullptr));
	}
}