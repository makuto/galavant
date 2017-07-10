#pragma once

#include <map>
#include <cassert>

#include "StringHashing.hpp"

#define RESKEY(x) COMPILE_TIME_CRC32_STR(x)

typedef int ResourceKey;

template <class ResourceType>
struct ResourceDictionary
{
	std::map<ResourceKey, ResourceType*> Resources;

	ResourceType* GetResource(ResourceKey resourceKey)
	{
		typename std::map<ResourceKey, ResourceType*>::iterator findIt =
		    Resources.find(resourceKey);
		if (findIt == Resources.end())
			return nullptr;
		return findIt->second;
	}
	void AddResource(ResourceKey resourceKey, ResourceType* resource)
	{
		typename std::map<ResourceKey, ResourceType*>::iterator findIt =
		    Resources.find(resourceKey);
		assert(
		    findIt ==
		    Resources.end() /*Tried to add a resource with key that is already in the dictionary*/);
		Resources[resourceKey] = resource;
	}
	void ClearResources()
	{
		Resources.clear();
	}
};