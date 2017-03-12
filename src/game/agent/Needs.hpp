#pragma once

#include <vector>

#include "../../world/WorldResourceLocator.hpp"

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
	const char* Name;

	float UpdateRate;

	float AddPerUpdate;

	NeedLevelTriggerList LevelTriggers;
};

struct Need
{
	NeedDef* Def;
	float Level;
	float LastUpdateTime;
};

typedef std::vector<Need> NeedList;
}