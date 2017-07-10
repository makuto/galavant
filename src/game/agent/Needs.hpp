#pragma once

#include "NeedTypes.hpp"

#include <vector>

#include "world/WorldResourceLocator.hpp"
#include "util/ResourceDictionary.hpp"

namespace gv
{
struct NeedLevelTrigger
{
	// Conditions
	bool GreaterThanLevel;

	float Level;

	// Actions
	bool NeedsResource;
	WorldResourceType WorldResource;

	bool DieNow;
};

typedef std::vector<NeedLevelTrigger> NeedLevelTriggerList;

struct NeedDef
{
	NeedType Type = gv::NeedType::None;

	const char* Name;

	float UpdateRate;

	float AddPerUpdate;

	NeedLevelTriggerList LevelTriggers;
};

struct Need
{
	NeedDef* Def = nullptr;

	float Level = 0.f;
	float LastUpdateTime = 0.f;
};

typedef std::vector<Need> NeedList;

extern ResourceDictionary<NeedDef> g_NeedDefDictionary;
}