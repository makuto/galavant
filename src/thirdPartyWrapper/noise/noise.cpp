#ifndef NOISE_CPP
#define NOISE_CPP

#include "noise.hpp"

#include "../../../thirdParty/OpenSimplexNoise/OpenSimplexNoise.hh"

class gv::noiseHolder
{
public:
	noiseHolder();
	virtual ~noiseHolder();
	virtual float eval(float x, float y);
};

gv::noiseHolder::noiseHolder()
{
}
gv::noiseHolder::~noiseHolder()
{
}
float gv::noiseHolder::eval(float x, float y)
{
	return 0;
}

class noise2dHolder : public gv::noiseHolder
{
private:
	OSN::Noise<2> noise;  // Internal OpenSimplex noise implementation
public:
	noise2dHolder(int seed) : noise(seed)
	{
	}
	~noise2dHolder()
	{
	}
	virtual float eval(float x, float y)
	{
		return noise.eval<float>(x, y);
	}
};
gv::Noise2d::Noise2d(int seed)
{
	noise = new noise2dHolder(seed);
}

gv::Noise2d::~Noise2d()
{
	delete noise;
}

float gv::Noise2d::rawNoise2d(float x, float y)
{
	return noise->eval(x, y);
}

float scaleNoiseValue(float value, float lowBound, float highBound)
{
	// Because raw noise is between -1 and 1, we add 1 to the value to
	// get 0 to 2, so we need half range to get low bound to high bound
	float halfRange = (highBound - lowBound) / 2;
	float valueScaled = value + 1;
	valueScaled = (valueScaled * halfRange) + lowBound;
	if (valueScaled > highBound)
		valueScaled = highBound;
	return valueScaled;
}
float gv::Noise2d::scaledRawNoise2d(float x, float y, float lowBound, float highBound)
{
	float value = rawNoise2d(x, y);
	return scaleNoiseValue(value, lowBound, highBound);
}

float gv::Noise2d::scaledOctaveNoise2d(float x, float y, float lowBound, float highBound,
									   int octaves, float scale, float persistence,
									   float lacunarity)
{
	float frequency = scale;
	float amplitude = 1;
	float maxAmplitude = 0;
	float value = 0;

	for (int i = 0; i < octaves; i++)
	{
		value += rawNoise2d(x * frequency, y * frequency) * amplitude;

		frequency *= lacunarity;
		maxAmplitude += amplitude;
		amplitude *= persistence;
	}

	return scaleNoiseValue(value / maxAmplitude, lowBound, highBound);
}

#endif
