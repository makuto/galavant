#include <iostream>
#include "../../thirdPartyWrapper/noise/noise.hpp"

void test2dNoise()
{
	const int SEED = 12412;
	std::cout << "Testing 2d noise\n";
	gv::Noise2d testNoise(SEED);
	std::cout << "Printing 100 noise values...\n";
	for (int i = 0; i < 100; i++)
	{
		float value = testNoise.scaledRawNoise2d(i, i, 0, 255);
		std::cout << "\t" << value << "\n";
	}
	std::cout << "Done\n";
}

void test3dNoise()
{
	const int SEED = 12412;
	std::cout << "Testing 3d noise\n";
	gv::Noise3d testNoise(SEED);
	std::cout << "Printing 100 noise values...\n";
	for (int i = 0; i < 100; i++)
	{
		float value = testNoise.scaledRawNoise3d(i, i, i, 0, 255);
		std::cout << "\t" << value << "\n";
	}
	std::cout << "Done\n";
}

int main()
{
	test2dNoise();
	test3dNoise();
	return 1;
}