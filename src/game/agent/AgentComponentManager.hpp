#pragma once

#include <vector>

#include "entityComponentSystem/PooledComponentManager.hpp"
#include "entityComponentSystem/EntityTypes.hpp"
#include "MovementManager.hpp"
#include "PlanComponentManager.hpp"
#include "Needs.hpp"

namespace gv
{
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
	GoalStatus Status = GoalStatus::None;

	int NumFailureRetries = 0;

	AgentGoalDef* Def = nullptr;

	WorldResourceType WorldResource = WorldResourceType::None;
};
typedef std::vector<AgentGoal> AgentGoalList;

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

	MovementManager* MovementComponentManager;

	EntityList UnconsciousAgents;

	void SetAgentConsciousState(PooledComponent<AgentComponentData>* agent,
	                            AgentConsciousState newState);

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

	void Initialize(PlanComponentManager* newPlanComponentManager,
	                MovementManager* newMovementComponentManager);

	virtual void Update(float deltaSeconds);

	void GetAgentConsciousStates(const EntityList& entities, AgentConsciousStateList& stateListOut);
	const EntityList& GetUnconsciousAgents();

	Need* GetAgentNeed(Entity entity, NeedType needType);
};

extern AgentComponentManager g_AgentComponentManager;
};