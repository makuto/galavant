#include "PlanComponentManager.hpp"

#include "../../util/Logging.hpp"

#include "../../entityComponentSystem/PooledComponentManager.hpp"
#include "../../entityComponentSystem/ComponentTypes.hpp"

namespace gv
{
PlanComponentManager::PlanComponentManager() : gv::PooledComponentManager<PlanComponentData>(100)
{
	Type = ComponentType::Plan;
}

PlanComponentManager::~PlanComponentManager()
{
}

void OnNotify(const Htn::TaskEventList& events, void* userData);

void PlanComponentManager::Initialize(WorldStateManager* newWorldStateManager,
                                      CallbackContainer<Htn::TaskEventCallback>* taskEventCallbacks)
{
	worldStateManager = newWorldStateManager;
	taskEventCallbacks->AddCallback(&OnNotify, this);
}

PlanExecuteStatus PlanComponentManager::ExecutePlan(Entity currentEntity,
                                                    PlanComponentData& currentComponent,
                                                    EntityList& entitiesToUnsubscribe)
{
	PlanExecuteStatus planStatus = PlanExecuteStatus::Running;
	Htn::TaskCallList::iterator currentStep = currentComponent.Planner.FinalCallList.begin();

	if (currentStep != currentComponent.Planner.FinalCallList.end())
	{
		Htn::ParameterList& parameters = (*currentStep).Parameters;
		Htn::Task* task = (*currentStep).TaskToCall;
		gv::WorldState& worldState = worldStateManager->GetWorldStateForAgent(currentEntity);

		if (task)
		{
			Htn::TaskExecuteStatus status;

			if (currentComponent.WaitingForEvent)
			{
				for (Htn::TaskEventList::iterator eventIt = ReceivedEvents.begin();
				     eventIt != ReceivedEvents.end(); ++eventIt)
				{
					const Htn::TaskEvent& event = (*eventIt);
					if (event.entity == currentEntity)
					{
						LOGD_IF(DebugPrint) << "Entity [" << event.entity
						                    << "] Task notify returned status "
						                    << (int)event.Result;
						// TODO: This is a hack; make event result mimic execution
						// status (we want the same results as that)
						status.Status = (event.Result == Htn::TaskEvent::TaskResult::TaskSucceeded ?
						                     Htn::TaskExecuteStatus::Succeeded :
						                     Htn::TaskExecuteStatus::Failed);

						ReceivedEvents.erase(eventIt);
						currentComponent.WaitingForEvent = false;
						break;
					}
				}

				// TODO: Part of the hack above; if we didn't get our event, fake as if
				// we ran and it called for a subscribe
				if (currentComponent.WaitingForEvent)
					status.Status = Htn::TaskExecuteStatus::WaitForEvent;
			}
			else
			{
				// We are ignorant of type here because the plan consists only of
				// primitives at this point
				status = task->GetPrimitive()->Execute(worldState, parameters);
			}

			switch (status.Status)
			{
				case Htn::TaskExecuteStatus::Reexecute:
					// Don't get rid of the task - execute again
					break;
				case Htn::TaskExecuteStatus::WaitForEvent:
					currentComponent.WaitingForEvent = true;
					break;
				case Htn::TaskExecuteStatus::Failed:
				{
					// On failure, unsubscribe. Don't try to replan or anything yet
					LOGD_IF(DebugPrint) << "Task failed for entity " << (int)currentEntity;
					planStatus = PlanExecuteStatus::Failed;
					// Flow through to default remove case
				}
				// All other statuses result in us getting rid of the task
				default:
				{
					LOGD_IF(DebugPrint) << "Task returned conclusive status " << status.Status;
					currentComponent.Planner.FinalCallList.erase(currentStep);

					// We have finished all tasks
					if (currentComponent.Planner.FinalCallList.empty())
						planStatus = PlanExecuteStatus::Succeeded;

					break;
				}
			}
		}
		else
			currentComponent.Planner.FinalCallList.erase(currentStep);
	}
	else
	{
		LOGE_IF(DebugPrint) << "Call list empty; something weird happened";
		planStatus = PlanExecuteStatus::Failed;
	}

	// We've finished in a conclusive way; unsubscribe the entity
	if (planStatus == PlanExecuteStatus::Succeeded || planStatus == PlanExecuteStatus::Failed)
		entitiesToUnsubscribe.push_back(currentEntity);

	return planStatus;
}

void PlanComponentManager::Update(float deltaSeconds)
{
	EntityList entitiesToUnsubscribe;

	// Things had their chance to read these
	PlanConclusiveExecutionEvents.clear();

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
		PlanExecuteStatus planStatus = PlanExecuteStatus::Running;
		bool shouldStartPlanner = (componentPlanner.CurrentStatus == Htn::Planner::Status::None);

		if (shouldStartPlanner || componentPlanner.IsPlannerRunning())
		{
			Htn::Planner::Status status = componentPlanner.PlanStep();
			if (!componentPlanner.IsPlannerRunning())
			{
				if (status == Htn::Planner::Status::PlanComplete)
				{
					if (DebugPrint)
					{
						LOGD << "Sucessful plan for Entity " << currentComponent->entity
						     << "! Final Call List:";
						Htn::PrintTaskCallList(componentPlanner.FinalCallList);
					}
					// We'll execute the plan next Update()
				}

				if (status < Htn::Planner::Status::Running_EnumBegin)
				{
					LOGD_IF(DebugPrint) << "Failed plan for Entity " << currentComponent->entity
					                    << " with code " << int(status) << "! Initial Call List:";
					Htn::PrintTaskCallList(componentPlanner.InitialCallList);

					// Plan failed, remove entity
					LOGD_IF(DebugPrint) << "Plan not running/failed";
					planStatus = PlanExecuteStatus::Failed;
					entitiesToUnsubscribe.push_back(currentEntity);
				}
			}
		}
		// Planning complete; resume execution or remove unsuccessful plans
		else
		{
			if (componentPlanner.CurrentStatus == Htn::Planner::Status::PlanComplete)
			{
				planStatus =
				    ExecutePlan(currentEntity, currentComponent->data, entitiesToUnsubscribe);
			}
			else
			{
				LOGD_IF(DebugPrint) << "Plan not complete, status "
				                    << (int)componentPlanner.CurrentStatus;
				entitiesToUnsubscribe.push_back(currentEntity);
			}
		}

		// Make sure we create an event if there was a conclusive status
		if (planStatus > PlanExecuteStatus::Begin_Conclusive)
		{
			PlanExecutionEvent executionEvent = {currentEntity, planStatus};
			PlanConclusiveExecutionEvents.push_back(executionEvent);
		}
	}

	// Because we just iterated through all things which could've read TaskEvents we've received, we
	// want to clear the rest out, because we don't care about them.
	// TODO: This is dirty because we have no guarantees that a task won't cause a TaskEvent. They
	// should never do that, but they could, then we would miss the event. This whole structure
	// needs to be rethought
	ReceivedEvents.clear();

	if (!entitiesToUnsubscribe.empty())
	{
		LOGD << "Unsubscribing " << entitiesToUnsubscribe.size() << " entities (we have "
		     << Subscribers.size() << " subscribers and " << PooledComponents.GetTotalActiveData()
		     << " components)";
		UnsubscribeEntities(entitiesToUnsubscribe);
	}
}

// @Callback: TaskEventCallback
void OnNotify(const Htn::TaskEventList& events, void* userData)
{
	if (userData)
	{
		PlanComponentManager* planManager = (PlanComponentManager*)userData;
		planManager->ReceivedEvents.insert(planManager->ReceivedEvents.begin(), events.begin(),
		                                   events.end());
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
		// @Performance: Could possibly make planner look directly at lists given instead of copying
		planner.InitialCallList.insert(planner.InitialCallList.end(), goalCallList.begin(),
		                               goalCallList.end());

		planner.DebugPrint = DebugPrint;
	}

	LOGD << "Subscribed " << subscribers.size() << " entities";
}

void PlanComponentManager::UnsubscribePoolEntitiesInternal(const EntityList& unsubscribers,
                                                           PlanComponentRefList& components)
{
	LOGD << "Unsubscribing " << unsubscribers.size() << " unsubscribers (" << components.size()
	     << " components)";
	for (gv::PooledComponent<PlanComponentData>* currentComponent : components)
	{
		if (!currentComponent)
			continue;

		// Perform unsubscription
	}
}

const PlanExecutionEventList& PlanComponentManager::GetConclusiveExecutionEvents() const
{
	return PlanConclusiveExecutionEvents;
}
}