#pragma once

#include "Position.hpp"
#include "thirdPartyWrapper/noise/noise.hpp"

#define WORLD_CELL_X_SIZE 16
#define WORLD_CELL_Y_SIZE 16
#define WORLD_CELL_Z_SIZE 255

namespace gv
{
namespace ProceduralWorld
{
struct ProceduralWorldParams
{
	int Seed;

	ScaledOctaveNoiseParams ScaledNoiseParams;

	// The size of a single 3D tile.
	float WorldCellTileSize[3];

	float WorldCellMaxHeight;
	float WorldCellMinHeight;
};

ProceduralWorldParams& GetActiveWorldParams();

// TODO: This assumes a 2D world. We need to figure the world out
struct WorldCellTileMap
{
	unsigned char TileHeights[WORLD_CELL_Y_SIZE][WORLD_CELL_X_SIZE];
};

void GetCellTileMapForPosition(const Position& cellPosition, WorldCellTileMap* tileMapOut);

// Sample (width * height) cell heights (e.g. for world map)
void SampleWorldCellHeights(const Position& center, int width, int height,
                            unsigned char* sampleHeights2d);
}
}