#include "WorldResourceLocator.hpp"
#include <limits>

namespace gv
{
namespace WorldResourceLocator
{
static const unsigned int DEFAULT_RESOURCELIST_SIZE = 10;

typedef std::map<WorldResourceType, ResourceList*> ResourceMap;
static ResourceMap s_Resources;

void ClearResources()
{
	for (std::pair<const WorldResourceType, ResourceList*>& resourceTypeList : s_Resources)
	{
		delete resourceTypeList.second;
	}
	s_Resources.clear();
}

bool ResourceListExists(const WorldResourceType type)
{
	return s_Resources.find(type) != s_Resources.end();
}

bool ResourceExistsInWorld(const WorldResourceType type)
{
	return ResourceListExists(type) && !s_Resources[type]->empty();
}

int NumResourcesInWorld(const WorldResourceType type)
{
	if (ResourceListExists(type))
		return s_Resources[type]->size();
	return 0;
}

void AddResource(const WorldResourceType type, const gv::Position& location)
{
	gv::Position newResource(location);
	// Ensure we're not exactly 0,0,0 because I designed this poorly
	newResource.Z = !newResource ? 0.1f : newResource.Z;

	if (ResourceListExists(type))
	{
		s_Resources[type]->push_back(newResource);
	}
	else
	{
		ResourceList* newResourceList = new ResourceList();
		newResourceList->reserve(DEFAULT_RESOURCELIST_SIZE);
		newResourceList->push_back(newResource);
		s_Resources[type] = newResourceList;
	}
}

void RemoveResource(const WorldResourceType type, const gv::Position& location)
{
	if (ResourceListExists(type))
	{
		ResourceList* resourceList = s_Resources[type];
		ResourceList::iterator resourceIt =
		    std::find(resourceList->begin(), resourceList->end(), location);
		if (resourceIt != resourceList->end())
			resourceList->erase(resourceIt);
	}
}

void MoveResource(const WorldResourceType type, const gv::Position& oldLocation,
                  const gv::Position& newLocation)
{
	if (ResourceListExists(type))
	{
		for (gv::Position& currentResource : *s_Resources[type])
		{
			// They should be exactly equal. It's the caller's responsibility to keep track of this
			if (currentResource.Equals(oldLocation, 1.f))
			{
				currentResource = newLocation;
				// Ensure we're not exactly 0,0,0 because I designed this poorly
				currentResource.Z = !currentResource ? 0.1f : currentResource.Z;
				break;
			}
		}
	}
}

// Find the nearest resource. Uses Manhattan distance
// Manhattan distance of -1 indicates no resource was found
gv::Position FindNearestResource(const WorldResourceType type, const gv::Position& location,
                                 bool allowSameLocation, float& manhattanToOut)
{
	gv::Position zeroPosition;
	if (ResourceListExists(type))
	{
		gv::Position closestResource;
		float closestResourceDistance = std::numeric_limits<float>::max();

		for (gv::Position& currentResource : *s_Resources[type])
		{
			float currentResourceDistance = location.ManhattanTo(currentResource);
			if (currentResourceDistance < closestResourceDistance &&
			    (allowSameLocation || currentResourceDistance > 0.f))
			{
				closestResourceDistance = currentResourceDistance;
				closestResource = currentResource;
			}
		}

		manhattanToOut = closestResourceDistance;
		return closestResource;
	}

	manhattanToOut = -1.f;
	return zeroPosition;
}
}
}