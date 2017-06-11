#pragma once

#include <map>
#include <vector>

#include "../entityComponentSystem/EntityTypes.hpp"
#include "Position.hpp"

namespace gv
{
enum WorldResourceType
{
	None = 0,

	Agent,
	Food,
	Player,

	WorldResourceType_count
};

namespace WorldResourceLocator
{
struct Resource
{
	Entity entity;
	Position position;
};
bool operator==(const Resource& a, const Resource& b);

typedef std::vector<Resource> ResourceList;

void ClearResources();

bool ResourceExistsInWorld(const WorldResourceType type);
int NumResourcesInWorld(const WorldResourceType type);

void AddResource(const WorldResourceType type, Entity entity, const Position& location);
void RemoveResource(const WorldResourceType type, Entity entity, const Position& location);
void MoveResource(const WorldResourceType type, Entity entity, const Position& oldLocation,
                  const Position& newLocation);

// Find the nearest resource. Uses Manhattan distance
// Returns nullptr if no reasource nearby
Resource* FindNearestResource(const WorldResourceType type, const Position& location,
                              bool allowSameLocation, float& manhattanToOut);

const ResourceList* GetResourceList(const WorldResourceType type);
}
}