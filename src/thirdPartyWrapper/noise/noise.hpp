#ifndef NOISE_HPP
#define NOISE_HPP

namespace gv
{
// Needed to avoid multiple defintions of OpenSimplex noise template metaprogram thingies
class noiseHolder;

/* Scaled Octave Noise Params:
 * See http://libnoise.sourceforge.net/glossary/#octave for a great
 * description of how this works
 * */
struct ScaledOctaveNoiseParams
{
	// The lowest and highest value to map the noise to
	float lowBound;
	float highBound;

	// The number of octaves to add (more = more detail, but slower)
	int octaves;

	// The starting scale of the first octave
	float scale;

	// Multiplied by the amplitude each octave, making subsequent octave values have less weight
	float persistence;

	// The scale is multiplied by this value every octave, changing the frequency. The true "octave"
	// value for lacunarity is 2
	float lacunarity;
};

/* --Noise2d--
 * This class uses OpenSimplex noise to generate noise given an x and y
 * coordinate.
 * */
class Noise2d
{
private:
	noiseHolder* noise;

public:
	Noise2d(int seed);
	~Noise2d();

	// Returns a noise value between -1 and 1
	float rawNoise2d(float x, float y);

	// Scales a raw noise value to be between lowBound and highBound
	float scaledRawNoise2d(float x, float y, float lowBound, float highBound);

	// See ScaledOctaveNoiseParams to know what each parameter does
	float scaledOctaveNoise2d(float x, float y, float lowBound, float highBound, int octaves,
	                          float scale, float persistence, float lacunarity);

	float scaledOctaveNoise2d(float x, float y, ScaledOctaveNoiseParams& params);
};

/* --Noise3d--
 * This class uses OpenSimplex noise to generate noise given an x and y
 * coordinate.
 * */
class Noise3d
{
private:
	noiseHolder* noise;

public:
	Noise3d(int seed);
	~Noise3d();

	// Returns a noise value between -1 and 1
	float rawNoise3d(float x, float y, float z);

	// Scales a raw noise value to be between lowBound and highBound
	float scaledRawNoise3d(float x, float y, float z, float lowBound, float highBound);

	// See ScaledOctaveNoiseParams to know what each parameter does
	float scaledOctaveNoise3d(float x, float y, float z, float lowBound, float highBound,
	                          int octaves, float scale, float persistence, float lacunarity);

	float scaledOctaveNoise3d(float x, float y, float z, ScaledOctaveNoiseParams& params);
};
}

#endif
