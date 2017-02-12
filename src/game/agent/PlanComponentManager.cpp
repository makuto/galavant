#include "PlanComponentManager.hpp"

#include "../../entityComponentSystem/PooledComponentManager.hpp"

PlanComponentManager::PlanComponentManager() : gv::PooledComponentManager<PlanComponentData>(100)
{
}

PlanComponentManager::~PlanComponentManager()
{
}

void PlanComponentManager::Initialize()
{
}

void PlanComponentManager::Update(float deltaSeconds)
{
	// TODO: Adding true iterator support to pool will drastically help damning this to hell
	gv::PooledComponentManager<PlanComponentData>::FragmentedPoolIterator it =
	    gv::PooledComponentManager<PlanComponentData>::NULL_POOL_ITERATOR;
	for (gv::PooledComponent<PlanComponentData>* currentComponent = ActivePoolBegin(it);
	     currentComponent != nullptr &&
	     it != gv::PooledComponentManager<PlanComponentData>::NULL_POOL_ITERATOR;
	     currentComponent = GetNextActivePooledComponent(it))
	{
		if (!currentComponent)
			continue;

		Htn::Planner& componentPlanner = currentComponent->data.Planner;

		// For now, don't follow plan, just ignore finished/failed plans
		if (!componentPlanner.IsPlanRunning())
			continue;

		Htn::Planner::Status status = componentPlanner.PlanStep();
		if (!componentPlanner.IsPlanRunning())
		{
			if (status == Htn::Planner::Status::PlanComplete)
			{
				std::cout << "PlanComponentManager: Sucessful plan for Entity "
				          << currentComponent->entity << "! Final Call List:\n";
				Htn::PrintTaskCallList(componentPlanner.FinalCallList);
			}

			if (status < Htn::Planner::Status::Running_EnumBegin)
			{
				std::cout << "PlanComponentManager: Failed plan for Entity "
				          << currentComponent->entity << "! Initial Call List:\n";
				Htn::PrintTaskCallList(componentPlanner.InitialCallList);
			}
		}
	}
}

void PlanComponentManager::SubscribeEntitiesInternal(PlanComponentRefList& components)
{
	for (gv::PooledComponent<PlanComponentData>* currentComponent : components)
	{
		if (!currentComponent)
			continue;

		Htn::Planner& planner = currentComponent->data.Planner;
		Htn::TaskCallList& goalCallList = currentComponent->data.Goals;

		planner.InitialCallList.insert(planner.InitialCallList.end(), goalCallList.begin(),
		                               goalCallList.end());
	}
}

void PlanComponentManager::UnsubscribeEntitiesInternal(PlanComponentRefList& components)
{
	for (gv::PooledComponent<PlanComponentData>* currentComponent : components)
	{
		if (!currentComponent)
			continue;

		// Perform unsubscription
	}
}