#include "WorldResourceLocator.hpp"
#include <limits>

namespace gv
{
namespace WorldResourceLocator
{
bool operator==(const Resource& a, const Resource& b)
{
	return (a.entity == b.entity && a.position.Equals(b.position, POSITION_TOLERANCE));
}

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

void AddResource(const WorldResourceType type, Entity entity, const gv::Position& location)
{
	Resource newResource = {entity, location};

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

void RemoveResource(const WorldResourceType type, Entity entity, const gv::Position& location)
{
	if (ResourceListExists(type))
	{
		ResourceList* resourceList = s_Resources[type];
		Resource resource = {entity, location};
		for (ResourceList::iterator resourceIt = resourceList->begin();
		     resourceIt != resourceList->end(); ++resourceIt)
		{
			if (resource == (*resourceIt))
			{
				resourceList->erase(resourceIt);
				break;
			}
		}
	}
}

void MoveResource(const WorldResourceType type, Entity entity, const gv::Position& oldLocation,
                  const gv::Position& newLocation)
{
	if (ResourceListExists(type))
	{
		Resource resource = {entity, oldLocation};
		for (Resource& currentResource : *s_Resources[type])
		{
			// They should be exactly equal. It's the caller's responsibility to keep track of this
			if (currentResource == resource)
			{
				currentResource.position = newLocation;
				break;
			}
		}
	}
}

// Find the nearest resource. Uses Manhattan distance
// Manhattan distance of -1 indicates no resource was found
Resource* FindNearestResource(const WorldResourceType type, const gv::Position& location,
                              bool allowSameLocation, float& manhattanToOut)
{
	if (ResourceListExists(type))
	{
		Resource* closestResource = nullptr;
		float closestResourceDistance = std::numeric_limits<float>::max();

		for (Resource& currentResource : *s_Resources[type])
		{
			float currentResourceDistance = location.ManhattanTo(currentResource.position);
			if (currentResourceDistance < closestResourceDistance &&
			    (allowSameLocation || currentResourceDistance > 0.f))
			{
				closestResourceDistance = currentResourceDistance;
				closestResource = &currentResource;
			}
		}

		manhattanToOut = closestResourceDistance;
		return closestResource;
	}

	manhattanToOut = -1.f;
	return nullptr;
}

const ResourceList* GetResourceList(const WorldResourceType type)
{
	if (ResourceListExists(type))
		return s_Resources[type];

	return nullptr;
}
}
}