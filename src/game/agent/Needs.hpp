#pragma once

#include "NeedTypes.hpp"
#include "ai/htn/HTNTypes.hpp"
#include "ai/htn/HTNTasks.hpp"

#include <vector>

#include "world/WorldResourceLocator.hpp"
#include "util/ResourceDictionary.hpp"

namespace gv
{
struct AgentGoalDef
{
	enum class GoalType
	{
		None = 0,

		HtnPlan,
		GetResource,

		GoalType_Count
	};
	GoalType Type;

	// TODO: Some sort of waiting period might be a good idea
	int NumRetriesIfFailed;

	//
	// Plan to achieve goal
	//
	Htn::TaskCallList Tasks;
};

extern ResourceDictionary<AgentGoalDef> g_AgentGoalDefDictionary;

struct Need;
struct NeedLevelTrigger
{
	//
	// Conditions
	//
	enum class ConditionType
	{
		None = 0,

		Zero,
		GreaterThanLevel,
		LessThanLevel
	};

	ConditionType Condition;

	float Level;

	//
	// Actions
	//
	bool NeedsResource;
	WorldResourceType WorldResource;

	bool DieNow;

	AgentGoalDef* GoalDef;

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

	Need() = default;
	// Construct a Need from a NeedDef
	Need(ResourceKey needDefKey);
	~Need() = default;
};

typedef std::vector<Need> NeedList;

extern ResourceDictionary<NeedDef> g_NeedDefDictionary;
}