#include "AgentComponentManager.hpp"

#include "../../util/Logging.hpp"

#include "../../entityComponentSystem/PooledComponentManager.hpp"
#include "../../entityComponentSystem/ComponentTypes.hpp"
#include "../../ai/htn/HTNTaskDb.hpp"

namespace gv
{
AgentComponentManager::AgentComponentManager() : gv::PooledComponentManager<AgentComponentData>(100)
{
	Type = gv::ComponentType::Agent;
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
	for (const AgentGoal& goal : goals)
	{
		if (goalToAdd.Type == goal.Type)
			return;
	}

	goals.push_back(goalToAdd);
}

void AgentComponentManager::Update(float deltaSeconds)
{
	EntityList entitiesToUnsubscribe;
	PlanComponentManager::PlanComponentList newPlans;

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
			continue;

		// Update Needs
		for (Need& need : needs)
		{
			if (need.Def)
			{
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
					need.LastUpdateTime = WorldTime;

					for (const NeedLevelTrigger& needLevelTrigger : need.Def->LevelTriggers)
					{
						bool needTriggerHit = (needLevelTrigger.GreaterThanLevel &&
						                       need.Level > needLevelTrigger.Level);

						if (needTriggerHit)
						{
							if (needLevelTrigger.NeedsResource && needLevelTrigger.WorldResource)
							{
								AgentGoal newNeedResourceGoal{AgentGoal::GoalStatus::Initialized,
								                              AgentGoal::GoalType::GetResource,
								                              needLevelTrigger.WorldResource};
								AddGoalIfUniqueType(goals, newNeedResourceGoal);
							}
							else if (needLevelTrigger.DieNow)
							{
								currentComponent->data.IsAlive = false;
								LOGD_IF(DebugPrint) << "Agent Entity " << currentEntity
								                    << " has died!";
							}
						}
					}
				}
			}
			else
				LOG_ERROR << "Need on entity " << currentEntity << " has no def!";
		}

		// Update head goal
		if (!goals.empty())
		{
			AgentGoalList::iterator headGoalIt = goals.begin();
			AgentGoal& goal = (*headGoalIt);
			switch (goal.Status)
			{
				// Start up the goal
				case AgentGoal::GoalStatus::Initialized:
					// TODO: This IsSubscribed call will go away once PlanManager manages multiple
					// plans for a single entity. For now, we'll just wait until the entity is no
					// longer subscribed before adding our goal
					if (PlanManager && !PlanManager->IsSubscribed(currentEntity))
					{
						if (goal.Type == AgentGoal::GoalType::GetResource)
						{
							gv::PooledComponent<PlanComponentData> newPlanComponent;
							Htn::Parameter resourceToFind;
							resourceToFind.IntValue = goal.WorldResource;
							resourceToFind.Type = Htn::Parameter::ParamType::Int;
							Htn::ParameterList parameters = {resourceToFind};
							Htn::TaskCall getResourceCall{
							    Htn::TaskDb::GetTask(Htn::TaskName::GetResource), parameters};
							Htn::TaskCallList getResourceTasks = {getResourceCall};
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
					{
						LOG_ERROR << "Agent trying to start goal but no Plan Manager is set!";
						goals.erase(headGoalIt);
					}
					break;
				case AgentGoal::GoalStatus::Failed:
				case AgentGoal::GoalStatus::Succeeded:
					goals.erase(headGoalIt);
					break;
				default:
					break;
			}
		}
	}

	if (PlanManager && !newPlans.empty())
		PlanManager->SubscribeEntities(newPlans);

	UnsubscribeEntities(entitiesToUnsubscribe);
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

void AgentComponentManager::UnsubscribeEntitiesInternal(const EntityList& unsubscribers,
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
}