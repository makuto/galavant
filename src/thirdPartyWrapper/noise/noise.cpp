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
	virtual float eval(float x, float y, float z);
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

float gv::noiseHolder::eval(float x, float y, float z)
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

float gv::Noise2d::scaledOctaveNoise2d(float x, float y, ScaledOctaveNoiseParams& params)
{
	return scaledOctaveNoise2d(x, y, params.lowBound, params.highBound, params.octaves,
	                           params.scale, params.persistence, params.lacunarity);
}

class noise3dHolder : public gv::noiseHolder
{
private:
	OSN::Noise<3> noise;  // Internal OpenSimplex noise implementation
public:
	noise3dHolder(int seed) : noise(seed)
	{
	}
	~noise3dHolder()
	{
	}
	virtual float eval(float x, float y, float z)
	{
		return noise.eval<float>(x, y, z);
	}
};

gv::Noise3d::Noise3d(int seed)
{
	noise = new noise3dHolder(seed);
}

gv::Noise3d::~Noise3d()
{
	delete noise;
}

float gv::Noise3d::rawNoise3d(float x, float y, float z)
{
	return noise->eval(x, y, z);
}

float gv::Noise3d::scaledRawNoise3d(float x, float y, float z, float lowBound, float highBound)
{
	float value = rawNoise3d(x, y, z);
	return scaleNoiseValue(value, lowBound, highBound);
}

float gv::Noise3d::scaledOctaveNoise3d(float x, float y, float z, float lowBound, float highBound,
                                       int octaves, float scale, float persistence,
                                       float lacunarity)
{
	float frequency = scale;
	float amplitude = 1;
	float maxAmplitude = 0;
	float value = 0;

	for (int i = 0; i < octaves; i++)
	{
		value += rawNoise3d(x * frequency, y * frequency, z * frequency) * amplitude;

		frequency *= lacunarity;
		maxAmplitude += amplitude;
		amplitude *= persistence;
	}

	return scaleNoiseValue(value / maxAmplitude, lowBound, highBound);
}

float gv::Noise3d::scaledOctaveNoise3d(float x, float y, float z, ScaledOctaveNoiseParams& params)
{
	return scaledOctaveNoise3d(x, y, z, params.lowBound, params.highBound, params.octaves,
	                           params.scale, params.persistence, params.lacunarity);
}
#endif
