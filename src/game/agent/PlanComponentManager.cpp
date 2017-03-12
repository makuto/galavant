#include "PlanComponentManager.hpp"

#include "../../util/Logging.hpp"

#include "../../entityComponentSystem/PooledComponentManager.hpp"

namespace gv
{
PlanComponentManager::PlanComponentManager() : gv::PooledComponentManager<PlanComponentData>(100)
{
}

PlanComponentManager::~PlanComponentManager()
{
}

void PlanComponentManager::Initialize(WorldStateManager* newWorldStateManager)
{
	worldStateManager = newWorldStateManager;
}

void OnNotify(const Htn::TaskEventList& events, void* userData);

void PlanComponentManager::Update(float deltaSeconds)
{
	EntityList entitiesToUnsubscribe;

	if (!worldStateManager)
		return;

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
		Entity currentEntity = currentComponent->entity;

		if (!componentPlanner.IsPlannerRunning())
		{
			if (componentPlanner.CurrentStatus == Htn::Planner::Status::PlanComplete)
			{
				Htn::TaskCallList::iterator currentStep = componentPlanner.FinalCallList.begin();

				if (currentStep != componentPlanner.FinalCallList.end())
				{
					Htn::ParameterList& parameters = (*currentStep).Parameters;
					Htn::Task* task = (*currentStep).TaskToCall;
					gv::WorldState& worldState =
					    worldStateManager->GetWorldStateForAgent(currentEntity);

					if (task)
					{
						Htn::TaskExecuteStatus status;
						// We are ignorant of type here because the plan consists only of primitives
						// at this point
						status = task->GetPrimitive()->Execute(worldState, parameters);

						switch (status.Status)
						{
							case Htn::TaskExecuteStatus::Reexecute:
								// Don't get rid of the task - execute again
								break;
							case Htn::TaskExecuteStatus::Subscribe:
								if (status.EventCallbackContainer)
									status.EventCallbackContainer->AddCallback(&OnNotify, this);
								componentPlanner.FinalCallList.erase(currentStep);
								break;

							// All other statuses result in us getting rid of the task
							default:
								LOGD_IF(DebugPrint) << "Task returned conclusive status "
								                    << status.Status;
								componentPlanner.FinalCallList.erase(currentStep);
								break;
						}
					}
					else
						componentPlanner.FinalCallList.erase(currentStep);
				}
				else
				{
					// We have finished all tasks; remove this entity from the manager
					// TODO: We'll eventually hook up some event shit
					LOGD_IF(DebugPrint) << "PlanComponentManager: Call list empty";
					entitiesToUnsubscribe.push_back(currentEntity);
				}
			}
			else
			{
				LOGD_IF(DebugPrint) << "PlanComponentManager: Plan not complete, status "
				                    << (int)componentPlanner.CurrentStatus;
				entitiesToUnsubscribe.push_back(currentEntity);
			}
		}
		else
		{
			Htn::Planner::Status status = componentPlanner.PlanStep();
			if (!componentPlanner.IsPlannerRunning())
			{
				if (status == Htn::Planner::Status::PlanComplete)
				{
					if (DebugPrint)
					{
						LOGD << "PlanComponentManager: Sucessful plan for Entity "
						     << currentComponent->entity << "! Final Call List:";
						Htn::PrintTaskCallList(componentPlanner.FinalCallList);
					}
					// We'll execute the plan next Update()
				}

				if (status < Htn::Planner::Status::Running_EnumBegin)
				{
					LOGD_IF(DebugPrint) << "PlanComponentManager: Failed plan for Entity "
					                    << currentComponent->entity << " with code " << int(status)
					                    << "! Initial Call List:";
					Htn::PrintTaskCallList(componentPlanner.InitialCallList);

					// Plan failed, remove entity
					// TODO: Hook up  events
					LOGD_IF(DebugPrint) << "PlanComponentManager: Plan not running/failed";
					entitiesToUnsubscribe.push_back(currentEntity);
				}
			}
		}
	}

	LOGD_IF(entitiesToUnsubscribe.size()) << "PlanComponentManager: Unsubscribed "
	                                      << entitiesToUnsubscribe.size() << " entities";
	UnsubscribeEntities(entitiesToUnsubscribe);
}

void OnNotify(const Htn::TaskEventList& events, void* userData)
{
	if (userData)
	{
		PlanComponentManager* planManager = (PlanComponentManager*)userData;
		for (const Htn::TaskEvent& event : events)
		{
			LOGD_IF(planManager->DebugPrint) << "Entity [" << event.entity
			                                 << "] Task notify returned status "
			                                 << (int)event.Result;
		}
	}
}

void PlanComponentManager::SubscribeEntitiesInternal(const EntityList& subscribers,
                                                     PlanComponentRefList& components)
{
	for (gv::PooledComponent<PlanComponentData>* currentComponent : components)
	{
		if (!currentComponent)
			continue;

		Htn::Planner& planner = currentComponent->data.Planner;
		Htn::TaskCallList& goalCallList = currentComponent->data.Tasks;

		planner.State = worldStateManager->GetWorldStateForAgent(currentComponent->entity);
		planner.InitialCallList.insert(planner.InitialCallList.end(), goalCallList.begin(),
		                               goalCallList.end());

		// TODO: This is not kosher
		planner.CurrentStatus = Htn::Planner::Status::Running_SuccessfulPrimitive;
		planner.DebugPrint = DebugPrint;
	}

	LOGD_IF(DebugPrint) << "PlanComponentManager: Subscribed " << subscribers.size() << " entities";
}

void PlanComponentManager::UnsubscribeEntitiesInternal(const EntityList& unsubscribers,
                                                       PlanComponentRefList& components)
{
	for (gv::PooledComponent<PlanComponentData>* currentComponent : components)
	{
		if (!currentComponent)
			continue;

		// Perform unsubscription
	}
}
}