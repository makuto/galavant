#ifndef NOISE_HPP
#define NOISE_HPP

namespace gv
{
// Needed to avoid multiple defintions of OpenSimplex noise template metaprogram thingies
class noiseHolder;

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

	/* Adds multiple layers of varying amplitudes together, then scales
	 * the value to be between lowBound and highBound
	 *
	 * Parameters:
	 * x, y                     The point
	 * lowBound, highBound      The lowest and highest value to map the noise to
	 * scale                    The starting scale of the first octave
	 * octaves                  The number of octaves to add
	 * persistence              Multiplied by the amplitude each octave, making
	 *                          subsequent octave values have less weight
	 * lacunarity               The scale is multiplied by this value every
	 *                          octave, changing the frequency. The true
	 *                          "octave" value for lacunarity is 2
	 *
	 * See http://libnoise.sourceforge.net/glossary/#octave for a great
	 * description of how this works
	 * */
	float scaledOctaveNoise2d(float x, float y, float lowBound, float highBound, int octaves,
							  float scale, float persistence, float lacunarity);
};
}

#endif
