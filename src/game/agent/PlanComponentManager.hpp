#pragma once

#include "../../entityComponentSystem/PooledComponentManager.hpp"
#include "../../ai/htn/HTNPlanner.hpp"
#include "../../ai/htn/HTNTypes.hpp"
#include "../../ai/WorldState.hpp"
#include "../../util/CallbackContainer.hpp"

namespace gv
{
struct PlanComponentData
{
	Htn::TaskCallList Tasks;

protected:
	friend class PlanComponentManager;
	Htn::Planner Planner;

	bool WaitingForEvent = false;
};

enum class PlanExecuteStatus
{
	None = 0,

	Running,

	Begin_Conclusive,
	Succeeded,
	Failed,
};

struct PlanExecutionEvent
{
	Entity entity;
	PlanExecuteStatus status;
};
typedef std::vector<PlanExecutionEvent> PlanExecutionEventList;

/* --PlanComponentManager--
Prepare, manage, and execute plan(s) for Entities.

TODO: PooledComponentManager is going to need to be modified in order to handle Entities with many
plans.
*/
class PlanComponentManager : public gv::PooledComponentManager<PlanComponentData>
{
private:
	WorldStateManager* worldStateManager;

	PlanExecutionEventList PlanConclusiveExecutionEvents;

	PlanExecuteStatus ExecutePlan(Entity currentEntity, PlanComponentData& currentComponent,
	                              EntityList& entitiesToUnsubscribe);

protected:
	typedef std::vector<gv::PooledComponent<PlanComponentData>*> PlanComponentRefList;

	virtual void SubscribeEntitiesInternal(const EntityList& subscribers,
	                                       PlanComponentRefList& components);
	virtual void UnsubscribePoolEntitiesInternal(const EntityList& unsubscribers,
	                                             PlanComponentRefList& components);

public:
	typedef std::vector<gv::PooledComponent<PlanComponentData>> PlanComponentList;

	bool DebugPrint = false;

	PlanComponentManager();
	virtual ~PlanComponentManager();
	void Initialize(WorldStateManager* newWorldStateManager,
	                CallbackContainer<Htn::TaskEventCallback>* taskEventCallbacks);
	virtual void Update(float deltaSeconds);

	const PlanExecutionEventList& GetConclusiveExecutionEvents() const;

	Htn::TaskEventList ReceivedEvents;
};
};