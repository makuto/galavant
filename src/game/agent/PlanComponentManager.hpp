#pragma once

#include "../../entityComponentSystem/PooledComponentManager.hpp"
#include "../../ai/htn/HTNPlanner.cpp"
#include "../../ai/WorldState.hpp"

struct PlanComponentData
{
	gv::WorldState state;

	Htn::TaskCallList Goals;

protected:
	friend class PlanComponentManager;
	Htn::Planner Planner;
};

/* --PlanComponentManager--
Prepare, manage, and execute plan(s) for Entities.

TODO: PooledComponentManager is going to need to be discarded in order to handle Entities with many
plans.
*/
class PlanComponentManager : public gv::PooledComponentManager<PlanComponentData>
{
protected:
	typedef std::vector<gv::PooledComponent<PlanComponentData>*> PlanComponentRefList;

	virtual void SubscribeEntitiesInternal(PlanComponentRefList& components);
	virtual void UnsubscribeEntitiesInternal(PlanComponentRefList& components);

public:
	typedef std::vector<gv::PooledComponent<PlanComponentData>> PlanComponentList;

	PlanComponentManager();
	virtual ~PlanComponentManager();
	void Initialize();
	virtual void Update(float deltaSeconds);
};