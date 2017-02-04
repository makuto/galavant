#include <iostream>

#define CATCH_CONFIG_MAIN
#include "../../thirdParty/Catch/single_include/catch.hpp"

#include "../world/Position.hpp"

TEST_CASE("Position and GlobalPosition")
{
	SECTION("Position Initialization")
	{
		gv::Position position;
		REQUIRE((position.X == 0.f && position.Y == 0.f && position.Z == 0.f));

		gv::Position position2(1.f, 1.f, 1.f);
		REQUIRE((position2.X == 1.f && position2.Y == 1.f && position2.Z == 1.f));
	}

	SECTION("Position Array Accessor")
	{
		gv::Position position(1.f, 2.f, 3.f);
		REQUIRE(
		    (position.X == position[0] && position.Y == position[1] && position.Z == position[2]));
	}

	SECTION("Position Equals")
	{
		gv::Position a(1.f, 2.f, 3.f);
		gv::Position b(1.08f, 2.08f, 3.08f);
		REQUIRE(a.Equals(b, 0.1f));
		REQUIRE(!a.Equals(b, 0.01f));
	}

	SECTION("Position Const Operators")
	{
		gv::Position a(1.f, 2.f, 3.f);
		gv::Position b(1.f, 2.f, 3.f);
		gv::Position expectedResult(2.f, 4.f, 6.f);
		gv::Position result = a + b;
		REQUIRE(result.Equals(expectedResult, 0.01f));
	}

	SECTION("Position Modifier Operators")
	{
		gv::Position a(1.f, 2.f, 3.f);
		gv::Position b(1.f, 2.f, 3.f);
		gv::Position expectedResult(2.f, 4.f, 6.f);
		a += b;
		REQUIRE(a.Equals(expectedResult, 0.01f));
	}
}