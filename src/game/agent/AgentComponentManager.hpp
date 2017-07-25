#pragma once

#include <vector>

#include "../../entityComponentSystem/PooledComponentManager.hpp"
#include "../../entityComponentSystem/EntityTypes.hpp"
#include "PlanComponentManager.hpp"
#include "Needs.hpp"

namespace gv
{
struct AgentGoalDef
{
	enum class GoalType
	{
		None = 0,
		GetResource,

		GoalType_Count
	};
	GoalType Type;

	// TODO: Some sort of waiting period might be a good idea
	int NumRetriesIfFailed;
};

extern ResourceDictionary<AgentGoalDef> g_AgentGoalDefDictionary;

struct AgentGoal
{
	enum class GoalStatus
	{
		None = 0,
		StartGoal,
		InProgress,
		Failed,
		Succeeded
	};
	GoalStatus Status;

	int NumFailureRetries;

	AgentGoalDef* Def;

	WorldResourceType WorldResource;
};
typedef std::vector<AgentGoal> AgentGoalList;

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

struct AgentComponentData
{
	bool IsAlive = true;
	AgentConsciousState ConsciousState;

	NeedList Needs;
	AgentGoalList Goals;
};

class AgentComponentManager : public PooledComponentManager<AgentComponentData>
{
private:
	// TODO: Eventually this will be something more complicated
	float WorldTime;

	PlanComponentManager* PlanManager;

protected:
	typedef std::vector<PooledComponent<AgentComponentData>*> AgentComponentRefList;

	virtual void SubscribeEntitiesInternal(const EntityList& subscribers,
	                                       AgentComponentRefList& components);
	virtual void UnsubscribePoolEntitiesInternal(const EntityList& unsubscribers,
	                                             AgentComponentRefList& components);

public:
	typedef std::vector<PooledComponent<AgentComponentData>> AgentComponentList;

	bool DebugPrint = false;

	AgentComponentManager();
	virtual ~AgentComponentManager();

	void Initialize(PlanComponentManager* newPlanComponentManager);

	virtual void Update(float deltaSeconds);

	void GetAgentConsciousStates(const EntityList& entities, AgentConsciousStateList& stateListOut);

	Need* GetAgentNeed(Entity entity, NeedType needType);
};
};