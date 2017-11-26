#include "CombatComponentManager.hpp"

#include "game/agent/AgentComponentManager.hpp"

#include "util/Time.hpp"

namespace gv
{
ResourceDictionary<CombatActionDef> g_CombatActionDefDictionary;
CombatComponentManager g_CombatComponentManager;

CombatComponentManager::CombatComponentManager()
{
	DebugName = "CombatComponentManager";
}

void CombatComponentManager::Initialize(CombatFxHandler* fxHandler)
{
	FxHandler = fxHandler;
}

void CombatComponentManager::UnsubscribeEntitiesInternal(const EntityList& entities)
{
	for (CombatantList::iterator it = Combatants.begin(); it != Combatants.end();)
	{
		Entity currentEntity = (*it).entity;
		bool foundEntity = false;
		for (const Entity& unsubscribeEntity : entities)
		{
			if (currentEntity == unsubscribeEntity)
			{
				it = Combatants.erase(it);
				foundEntity = true;
				break;
			}
		}

		if (!foundEntity)
			++it;
	}
}

void CombatComponentManager::CreateCombatants(const EntityList& entities,
                                              CombatantRefList& newCombatants)
{
	// Only create combatants which aren't already subscribed
	EntityList entitiesToSubscribe = entities;
	EntityListRemoveNonUniqueEntitiesInSuspect(Subscribers, entitiesToSubscribe);

	unsigned int endBeforeResize = Combatants.size();
	Combatants.resize(Combatants.size() + entitiesToSubscribe.size());

	int numEntitiesCreated = 0;
	for (unsigned int i = endBeforeResize; i < Combatants.size(); i++)
	{
		Combatant* newCombatant = &Combatants[i];
		newCombatant->entity = entitiesToSubscribe[numEntitiesCreated++];
		newCombatants.push_back(newCombatant);
	}

	// We've already made sure all entities in the list are unique
	EntityListAppendList(Subscribers, entitiesToSubscribe);
}

Combatant* CombatComponentManager::GetCombatant(Entity combatantEntity)
{
	for (Combatant& combatant : Combatants)
	{
		if (combatant.entity == combatantEntity)
			return &combatant;
	}

	return nullptr;
}

void CombatComponentManager::ActivateCombatAction(Entity combatant, CombatAction action)
{
	if (FxHandler)
		FxHandler->OnActivateCombatAction(combatant, action);
}

void CombatComponentManager::Update(float deltaSeconds)
{
	TimeToNextUpdate -= deltaSeconds;

	if (TimeToNextUpdate <= 0.f)
	{
		TimeToNextUpdate = 0.f;

		for (Combatant& combatant : Combatants)
		{
			if (combatant.State == Combatant::CombatState::None)
				continue;

			if (!combatant.CurrentAction.Def)
				continue;

			if (combatant.CurrentAction.Def->Type != CombatActionDef::CombatActionType::None)
			{
				float actionEndTime =
				    combatant.CurrentAction.ActivateTime + combatant.CurrentAction.Def->Duration;
				if (gv::GetGameplayTime() < actionEndTime)
				{
					combatant.CurrentAction = {nullptr, nullptr, 0.f};
				}
			}
		}
	}
}

void CombatComponentManager::DamageDealerHitEntity(Entity hitEntity)
{
	for (Combatant& combatant : Combatants)
	{
		if (combatant.entity == hitEntity)
		{
			Need* bloodNeed = g_AgentComponentManager.GetAgentNeed(hitEntity, NeedType::Blood);
			if (!bloodNeed)
				break;

			bloodNeed->Level -= 20.f;

			break;
		}
	}
}
}