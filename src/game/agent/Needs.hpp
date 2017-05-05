#pragma once

#include "NeedTypes.hpp"

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
	NeedType Type;

	const char* Name;

	float UpdateRate;

	float AddPerUpdate;

	NeedLevelTriggerList LevelTriggers;
};

struct Need
{
	NeedType Type = gv::NeedType::None;
	NeedDef* Def = nullptr;

	float Level = 0;
	float LastUpdateTime = 0;
};

typedef std::vector<Need> NeedList;
}