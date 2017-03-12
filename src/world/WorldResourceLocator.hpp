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
	BusStop = 2
};

typedef std::vector<gv::Position> ResourceList;

void WorldResourceLocator_ClearResources();

bool WorldResourceLocator_ResourceExistsInWorld(const WorldResourceType type);

void WorldResourceLocator_AddResource(const WorldResourceType type, const gv::Position& location);
void WorldResourceLocator_RemoveResource(const WorldResourceType type,
                                         const gv::Position& location);
void WorldResourceLocator_MoveResource(const WorldResourceType type,
                                       const gv::Position& oldLocation,
                                       const gv::Position& newLocation);

// Find the nearest resource. Uses Manhattan distance
// Manhattan distance of -1 indicates no resource was found
gv::Position WorldResourceLocator_FindNearestResource(const WorldResourceType type,
                                                      const gv::Position& location,
                                                      bool allowSameLocation,
                                                      float& manhattanToOut);
}