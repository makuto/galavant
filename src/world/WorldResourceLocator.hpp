#pragma once

#include <map>
#include <vector>

#include "Position.hpp"

namespace gv
{
enum WorldResourceType
{
	None = 0,
	
	Agent = 1,
	Food = 2,

	WorldResourceType_count
};

typedef std::vector<gv::Position> ResourceList;

namespace WorldResourceLocator
{
void ClearResources();

bool ResourceExistsInWorld(const WorldResourceType type);
int NumResourcesInWorld(const WorldResourceType type);

void AddResource(const WorldResourceType type, const gv::Position& location);
void RemoveResource(const WorldResourceType type, const gv::Position& location);
void MoveResource(const WorldResourceType type, const gv::Position& oldLocation,
                  const gv::Position& newLocation);

// Find the nearest resource. Uses Manhattan distance
// Manhattan distance of -1 indicates no resource was found
gv::Position FindNearestResource(const WorldResourceType type, const gv::Position& location,
                                 bool allowSameLocation, float& manhattanToOut);
}
}