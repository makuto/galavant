#include "InteractComponentManager.hpp"

#include "entityComponentSystem/EntityComponentManager.hpp"
#include "agent/AgentComponentManager.hpp"

namespace gv
{
InteractComponentManager g_InteractComponentManager;

InteractComponentManager::InteractComponentManager()
    : ComponentManager("InteractComponentManager")
{
}

void InteractComponentManager::UnsubscribeEntitiesInternal(const EntityList& entities)
{
	for (PickupList::iterator it = Pickups.begin(); it != Pickups.end();)
	{
		Entity currentEntity = (*it).entity;
		bool foundEntity = false;
		for (const Entity& unsubscribeEntity : entities)
		{
			if (currentEntity == unsubscribeEntity)
			{
				it = Pickups.erase(it);
				foundEntity = true;
				break;
			}
		}

		if (!foundEntity)
			++it;
	}
}

void InteractComponentManager::CreatePickups(const EntityList& entities, PickupRefList& newPickups)
{
	EntityList entitiesToSubscribe = entities;
	EntityListRemoveNonUniqueEntitiesInSuspect(Subscribers, entitiesToSubscribe);

	unsigned int endBeforeResize = Pickups.size();
	Pickups.resize(Pickups.size() + entitiesToSubscribe.size());

	int numEntitiesCreated = 0;
	for (unsigned int i = endBeforeResize; i < Pickups.size(); i++)
	{
		Pickup* newPickup = &Pickups[i];
		newPickup->entity = entitiesToSubscribe[numEntitiesCreated++];
		newPickups.push_back(newPickup);
	}

	EntityListAppendList(Subscribers, entitiesToSubscribe);
}

Pickup* InteractComponentManager::GetPickup(Entity pickupEntity)
{
	for (Pickup& pickup : Pickups)
	{
		if (pickup.entity == pickupEntity)
			return &pickup;
	}

	return nullptr;
}

bool InteractComponentManager::PickupDirect(Entity pickupEntity, Entity claimer)
{
	if (EntityListFindEntity(Subscribers, pickupEntity))
	{
		// TODO: This will eventually pipe pickups into inventory, but for testing, we're going to
		// put the pickup directly into the need
		if (g_AgentComponentManager.IsSubscribed(claimer))
		{
			Pickup* pickup = GetPickup(pickupEntity);

			if (!pickup)
				return false;

			Need* needPickupAffects =
			    g_AgentComponentManager.GetAgentNeed(claimer, pickup->AffectsNeed);

			// TODO: For testing only; Directly contribute to need on pickup
			if (needPickupAffects)
			{
				needPickupAffects->Level -= 100;
				LOGD << "Entity " << claimer << " restored need " << needPickupAffects->Def->Name
				     << " by 100 picking up " << pickupEntity;
			}

			EntityList entitiesPickedUp;
			entitiesPickedUp.push_back(pickupEntity);
			UnsubscribeEntities(entitiesPickedUp);

			if (pickup->DestroySelfOnPickup)
				g_EntityComponentManager.MarkDestroyEntities(entitiesPickedUp);

			return true;
		}
	}
	return false;
}
}