#pragma once

#include "NeedTypes.hpp"

#include <vector>

#include "world/WorldResourceLocator.hpp"
#include "util/ResourceDictionary.hpp"

namespace gv
{
struct Need;
struct NeedLevelTrigger
{
	// Conditions
	enum class ConditionType
	{
		None = 0,

		Zero,
		GreaterThanLevel,
		LessThanLevel
	};

	ConditionType Condition;

	float Level;

	// Actions
	bool NeedsResource;
	WorldResourceType WorldResource;

	bool DieNow;

	bool ConditionsMet(Need& need) const;
};

typedef std::vector<NeedLevelTrigger> NeedLevelTriggerList;

struct NeedDef
{
	NeedType Type = gv::NeedType::None;

	const char* Name;

	float InitialLevel;
	float MaxLevel;
	float MinLevel;

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