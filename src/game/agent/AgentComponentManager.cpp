#include "AgentComponentManager.hpp"

#include "util/Logging.hpp"

#include "entityComponentSystem/PooledComponentManager.hpp"
#include "entityComponentSystem/EntityComponentManager.hpp"

#include "util/Math.hpp"

namespace gv
{
ResourceDictionary<AgentGoalDef> g_AgentGoalDefDictionary;
AgentComponentManager g_AgentComponentManager;

AgentComponentManager::AgentComponentManager() : gv::PooledComponentManager<AgentComponentData>(100)
{
	DebugName = "AgentComponentManager";
}

AgentComponentManager::~AgentComponentManager()
{
}

void AgentComponentManager::Initialize(PlanComponentManager* newPlanComponentManager)
{
	PlanManager = newPlanComponentManager;
}

void AddGoalIfUniqueType(AgentGoalList& goals, AgentGoal& goalToAdd)
{
	if (!goalToAdd.Def)
	{
		LOGE << "Tried to add AgentGoal with no def!";
		return;
	}

	for (const AgentGoal& goal : goals)
	{
		if (goalToAdd.Def->Type == goal.Def->Type)
			return;
	}

	goals.push_back(goalToAdd);
}

void AgentComponentManager::Update(float deltaSeconds)
{
	EntityList entitiesToUnsubscribe;
	EntityList entitiesToDestroy;
	PlanComponentManager::PlanComponentList newPlans;

	if (!PlanManager)
	{
		LOGE << "Cannot update Agents without PlanManager!";
		return;
	}

	const PlanExecutionEventList& planConclusiveExecutionEvents =
	    PlanManager->GetConclusiveExecutionEvents();

	WorldTime += deltaSeconds;

	// TODO: Adding true iterator support to pool will drastically help damning this to hell
	gv::PooledComponentManager<AgentComponentData>::FragmentedPoolIterator it =
	    gv::PooledComponentManager<AgentComponentData>::NULL_POOL_ITERATOR;
	for (gv::PooledComponent<AgentComponentData>* currentComponent = ActivePoolBegin(it);
	     currentComponent != nullptr &&
	     it != gv::PooledComponentManager<AgentComponentData>::NULL_POOL_ITERATOR;
	     currentComponent = GetNextActivePooledComponent(it))
	{
		if (!currentComponent)
			continue;

		Entity currentEntity = currentComponent->entity;
		NeedList& needs = currentComponent->data.Needs;
		AgentGoalList& goals = currentComponent->data.Goals;

		if (!currentComponent->data.IsAlive)
		{
			// TODO: Figure out what to do about dead agents/bodies
			entitiesToDestroy.push_back(currentEntity);
			continue;
		}

		// Update Needs
		for (Need& need : needs)
		{
			if (!need.Def)
			{
				LOG_ERROR << "Need on entity " << currentEntity << " has no def!";
				continue;
			}

			bool needUpdated = false;
			float updateDelta = WorldTime - need.LastUpdateTime;
			while (updateDelta >= need.Def->UpdateRate)
			{
				need.Level += need.Def->AddPerUpdate;

				updateDelta -= need.Def->UpdateRate;
				needUpdated = true;
			}

			if (needUpdated)
			{
				CLAMP(need.Level, need.Def->MinLevel, need.Def->MaxLevel);

				need.LastUpdateTime = WorldTime;

				LOGV_IF(DebugPrint) << "Agent Entity " << currentEntity << " updated need "
				                    << need.Def->Name << " to level " << need.Level;

				for (const NeedLevelTrigger& needLevelTrigger : need.Def->LevelTriggers)
				{
					if (!needLevelTrigger.ConditionsMet(need))
						continue;

					LOGD_IF(DebugPrint) << "Agent Entity " << currentEntity
					                    << " has hit need trigger for need " << need.Def->Name;

					if (needLevelTrigger.SetConsciousState != AgentConsciousState::None)
					{
						currentComponent->data.ConsciousState = needLevelTrigger.SetConsciousState;

						if (currentComponent->data.ConsciousState == AgentConsciousState::Dead)
						{
							currentComponent->data.IsAlive = false;
							LOGD_IF(DebugPrint) << "Agent Entity " << currentEntity
							                    << " has died from need " << need.Def->Name;
						}
					}

					if (needLevelTrigger.GoalDef)
					{
						AgentGoal newGoal;
						newGoal.Def = needLevelTrigger.GoalDef;
						AddGoalIfUniqueType(goals, newGoal);
					}
					else if (needLevelTrigger.NeedsResource && needLevelTrigger.WorldResource)
					{
						AgentGoal newNeedResourceGoal;
						newNeedResourceGoal.Def =
						    gv::g_AgentGoalDefDictionary.GetResource(RESKEY("GetResource"));
						newNeedResourceGoal.WorldResource = needLevelTrigger.WorldResource;
						AddGoalIfUniqueType(goals, newNeedResourceGoal);
					}
				}
			}
		}

		// Update head goal
		if (!goals.empty())
		{
			AgentGoalList::iterator headGoalIt = goals.begin();
			AgentGoal& goal = (*headGoalIt);
			switch (goal.Status)
			{
				case AgentGoal::GoalStatus::StartGoal:
					// TODO: This IsSubscribed call will go away once PlanManager manages multiple
					// plans for a single entity. For now, we'll just wait until the entity is no
					// longer subscribed before adding our goal
					if (!PlanManager->IsSubscribed(currentEntity))
					{
						if (goal.Def->Type == AgentGoalDef::GoalType::HtnPlan)
						{
							// TODO: @Purity Move validation to somewhere cleaner
							if (goal.Def->Tasks.empty())
								LOGE << "AgentGoalDef marked as a plan but has no Tasks!";

							gv::PooledComponent<PlanComponentData> newPlanComponent;
							newPlanComponent.entity = currentEntity;
							newPlanComponent.data.Tasks.insert(newPlanComponent.data.Tasks.end(),
							                                   goal.Def->Tasks.begin(),
							                                   goal.Def->Tasks.end());

							newPlans.push_back(newPlanComponent);
						}
						else if (goal.Def->Type == AgentGoalDef::GoalType::GetResource)
						{
							Htn::Parameter resourceToFind;
							resourceToFind.IntValue = goal.WorldResource;
							resourceToFind.Type = Htn::Parameter::ParamType::Int;
							Htn::ParameterList parameters = {resourceToFind};
							Htn::TaskCall getResourceCall{
							    Htn::g_TaskDictionary.GetResource(RESKEY("GetResource")),
							    parameters};
							Htn::TaskCall pickupResourceCall{
							    Htn::g_TaskDictionary.GetResource(RESKEY("InteractPickup")),
							    parameters};
							Htn::TaskCallList getResourceTasks = {getResourceCall,
							                                      pickupResourceCall};

							gv::PooledComponent<PlanComponentData> newPlanComponent;
							newPlanComponent.entity = currentEntity;
							newPlanComponent.data.Tasks.insert(newPlanComponent.data.Tasks.end(),
							                                   getResourceTasks.begin(),
							                                   getResourceTasks.end());

							newPlans.push_back(newPlanComponent);

							LOGD_IF(DebugPrint) << "Agent starting GetResource goal plan";
							goal.Status = AgentGoal::GoalStatus::InProgress;
						}
					}
					else
						LOGE << "Agent tried to start goal when already Planning!";
					break;
				case AgentGoal::GoalStatus::InProgress:
				{
					// While goal is in progress, watch planner events for a conclusive status
					bool entityConcludedPlan = false;
					for (PlanExecutionEvent planEvent : planConclusiveExecutionEvents)
					{
						if (planEvent.entity != currentEntity)
							continue;

						// If we got this far we received a relevant conclusive event
						entityConcludedPlan = true;

						if (planEvent.status == PlanExecuteStatus::Failed)
						{
							if (goal.Def->NumRetriesIfFailed &&
							    goal.NumFailureRetries < goal.Def->NumRetriesIfFailed)
							{
								LOGD_IF(DebugPrint)
								    << "Entity " << currentEntity << " retrying goal (tried "
								    << goal.NumFailureRetries << " times already; "
								    << goal.Def->NumRetriesIfFailed << " max tries)";

								goal.NumFailureRetries++;
								goal.Status = AgentGoal::GoalStatus::StartGoal;
								entityConcludedPlan = false;
							}
							else
								goal.Status = AgentGoal::GoalStatus::Failed;
						}
						else if (planEvent.status == PlanExecuteStatus::Succeeded)
							goal.Status = AgentGoal::GoalStatus::Succeeded;

						break;
					}

					// Fall through if we finished the plan (failed or otherwise)
					if (!entityConcludedPlan)
						break;
				}
				case AgentGoal::GoalStatus::Failed:
				case AgentGoal::GoalStatus::Succeeded:
				default:
					goals.erase(headGoalIt);
					break;
			}
		}
	}

	if (!newPlans.empty())
		PlanManager->SubscribeEntities(newPlans);

	if (!entitiesToUnsubscribe.empty())
		UnsubscribeEntities(entitiesToUnsubscribe);

	if (!entitiesToDestroy.empty())
	{
		g_EntityComponentManager.MarkDestroyEntities(entitiesToDestroy);
	}
}

void AgentComponentManager::SubscribeEntitiesInternal(const EntityList& subscribers,
                                                      AgentComponentRefList& components)
{
	for (gv::PooledComponent<AgentComponentData>* currentComponent : components)
	{
		if (!currentComponent)
			continue;

		currentComponent->data.ConsciousState = AgentConsciousState::Conscious;
	}

	LOGD_IF(DebugPrint) << "AgentComponentManager: Subscribed " << subscribers.size()
	                    << " entities";
}

void AgentComponentManager::UnsubscribePoolEntitiesInternal(const EntityList& unsubscribers,
                                                            AgentComponentRefList& components)
{
	for (gv::PooledComponent<AgentComponentData>* currentComponent : components)
	{
		if (!currentComponent)
			continue;

		// Perform unsubscription
	}

	LOGD_IF(unsubscribers.size()) << "AgentComponentManager: Unsubscribed " << unsubscribers.size()
	                              << " entities";
}

void AgentComponentManager::GetAgentConsciousStates(const EntityList& entities,
                                                    AgentConsciousStateList& stateListOut)
{
	for (const Entity& entity : entities)
	{
		bool foundEntity = false;

		// TODO: Adding true iterator support to pool will drastically help damning this to hell
		gv::PooledComponentManager<AgentComponentData>::FragmentedPoolIterator it =
		    gv::PooledComponentManager<AgentComponentData>::NULL_POOL_ITERATOR;
		for (gv::PooledComponent<AgentComponentData>* currentComponent = ActivePoolBegin(it);
		     currentComponent != nullptr &&
		     it != gv::PooledComponentManager<AgentComponentData>::NULL_POOL_ITERATOR;
		     currentComponent = GetNextActivePooledComponent(it))
		{
			if (!currentComponent)
				continue;

			Entity currentEntity = currentComponent->entity;

			if (currentEntity == entity)
			{
				stateListOut.push_back(currentComponent->data.ConsciousState);
				foundEntity = true;
				break;
			}
		}

		if (!foundEntity)
			stateListOut.push_back(AgentConsciousState::None);
	}
}

Need* AgentComponentManager::GetAgentNeed(Entity entity, NeedType needType)
{
	// TODO: Adding true iterator support to pool will drastically help damning this to hell
	gv::PooledComponentManager<AgentComponentData>::FragmentedPoolIterator it =
	    gv::PooledComponentManager<AgentComponentData>::NULL_POOL_ITERATOR;
	for (gv::PooledComponent<AgentComponentData>* currentComponent = ActivePoolBegin(it);
	     currentComponent != nullptr &&
	     it != gv::PooledComponentManager<AgentComponentData>::NULL_POOL_ITERATOR;
	     currentComponent = GetNextActivePooledComponent(it))
	{
		if (!currentComponent)
			continue;

		Entity currentEntity = currentComponent->entity;

		if (currentEntity == entity)
		{
			for (Need& need : currentComponent->data.Needs)
			{
				if (need.Def->Type == needType)
					return &need;
			}
		}
	}

	return nullptr;
}
}