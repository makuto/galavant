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
	GoalType Type = GoalType::None;

	// TODO: Some sort of waiting period might be a good idea
	int NumRetriesIfFailed = 0;

	//
	// Plan to achieve goal
	//
	Htn::TaskCallList Tasks;
};

extern ResourceDictionary<AgentGoalDef> g_AgentGoalDefDictionary;

// An agent can only be one of these at a time
enum class AgentConsciousState
{
	None = 0,

	Conscious,
	Unconscious,
	Sleeping,
	Dead,

	AgentState_count
};

typedef std::vector<AgentConsciousState> AgentConsciousStateList;

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

	ConditionType Condition = ConditionType::None;

	float Level = 0.f;

	//
	// Actions
	//
	bool NeedsResource = false;
	WorldResourceType WorldResource = WorldResourceType::None;

	AgentConsciousState SetConsciousState = AgentConsciousState::None;

	AgentGoalDef* GoalDef = nullptr;

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