#pragma once

#include <vector>

#include "../../entityComponentSystem/PooledComponentManager.hpp"
#include "../../entityComponentSystem/EntityTypes.hpp"
#include "PlanComponentManager.hpp"
#include "Needs.hpp"

namespace gv
{
struct AgentGoal
{
	enum class GoalStatus
	{
		None = 0,
		Initialized,
		InProgress,
		Failed,
		Succeeded
	};
	GoalStatus Status;

	enum class GoalType
	{
		None = 0,
		GetResource,

		GoalType_Count
	};
	GoalType Type;

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
	virtual void UnsubscribeEntitiesInternal(const EntityList& unsubscribers,
	                                         AgentComponentRefList& components);

public:
	typedef std::vector<PooledComponent<AgentComponentData>> AgentComponentList;

	bool DebugPrint = false;

	AgentComponentManager();
	virtual ~AgentComponentManager();

	void Initialize(PlanComponentManager* newPlanComponentManager);

	virtual void Update(float deltaSeconds);

	void GetAgentConsciousStates(const EntityList& entities, AgentConsciousStateList& stateListOut);
};
};