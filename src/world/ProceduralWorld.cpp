#include "ProceduralWorld.hpp"

namespace gv
{
namespace ProceduralWorld
{
static ProceduralWorldParams s_WorldParams;

// This isn't fantastic, but I can replace it with a better system once one is needed
ProceduralWorldParams& GetActiveWorldParams()
{
	return s_WorldParams;
}

static float GetTileHeightForWorldPosition(const Position& tileWorldPosition)
{
	static int seed = 0;
	static gv::Noise2d* noise2dGenerator = nullptr;

	if (!noise2dGenerator || seed != s_WorldParams.Seed)
	{
		seed = s_WorldParams.Seed;
		noise2dGenerator = new gv::Noise2d(seed);
	}

	/*float tileHeight = noise2dGenerator->scaledOctaveNoise2d(
	    tileWorldPosition.X, tileWorldPosition.Y, s_WorldParams.ScaledNoiseParams);*/

	float tileHeight = 128.f;  // FOR DEBUGGING ONLY; DELETE ME!

	return tileHeight;
}

void GetCellTileMapForPosition(const Position& cellPosition, WorldCellTileMap* tileMapOut)
{
	if (!tileMapOut)
		return;

	for (int tileY = 0; tileY < WORLD_CELL_Y_SIZE; tileY++)
	{
		for (int tileX = 0; tileX < WORLD_CELL_X_SIZE; tileX++)
		{
			// Don't ever care about Z axis because we're working in 2D
			int tileCoord[] = {tileX, tileY, 0};

			Position tileWorldPosition(cellPosition);
			for (int i = 0; i < 3; i++)
				tileWorldPosition[i] += tileCoord[i] * s_WorldParams.WorldCellTileSize[i];

			float tileHeight = GetTileHeightForWorldPosition(tileWorldPosition);

			// Remap tile height, if necessary
			if (s_WorldParams.ScaledNoiseParams.highBound != 255.f)
				tileHeight = (tileHeight / s_WorldParams.ScaledNoiseParams.highBound) * 255.f;

			tileMapOut->TileHeights[tileY][tileX] = (unsigned char)tileHeight;
		}
	}
}

void SampleWorldCellHeights(const Position& center, int width, int height,
                            unsigned char* sampleHeights2d)
{
	if (!sampleHeights2d)
		return;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int worldCellSize[] = {WORLD_CELL_X_SIZE, WORLD_CELL_Y_SIZE, WORLD_CELL_Z_SIZE};
			int sampleCoord[] = {x - (width / 2), y - (height / 2), 0};

			Position cellWorldPosition(center);
			for (int i = 0; i < 3; i++)
				cellWorldPosition[i] +=
				    sampleCoord[i] * (s_WorldParams.WorldCellTileSize[i] * worldCellSize[i]);

			float tileHeight = GetTileHeightForWorldPosition(cellWorldPosition);

			// Remap tile height, if necessary
			if (s_WorldParams.ScaledNoiseParams.highBound != 255.f)
				tileHeight = (tileHeight / s_WorldParams.ScaledNoiseParams.highBound) * 255.f;

			sampleHeights2d[(y * width) + x] = (unsigned char)tileHeight;
		}
	}
}
}
}