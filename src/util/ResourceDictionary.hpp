#pragma once

#include <vector>
#include <map>
#include <cassert>

#include "StringHashing.hpp"

#define RESKEY(x) COMPILE_TIME_CRC32_STR(x)

namespace gv
{
typedef int ResourceKey;

// This base is required because we want to store pointers to many types of ResourceDictionary
struct ResourceDictionaryBase
{
	static std::vector<ResourceDictionaryBase*> s_AllResourceDictionaries;
	virtual ~ResourceDictionaryBase() = default;
	virtual void ClearResources() = 0;

	static void ClearAllDictionaries()
	{
		for (ResourceDictionaryBase* dictionary : s_AllResourceDictionaries)
			dictionary->ClearResources();
	}
};

template <class ResourceType>
struct ResourceDictionary : public ResourceDictionaryBase
{
	std::map<ResourceKey, ResourceType*> Resources;

	ResourceDictionary()
	{
		s_AllResourceDictionaries.push_back(this);
	}

	virtual ~ResourceDictionary() = default;

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
	virtual void ClearResources()
	{
		Resources.clear();
	}
};
}