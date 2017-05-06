#include "InteractComponentManager.hpp"

#include "../entityComponentSystem/EntityComponentManager.hpp"
#include "agent/AgentComponentManager.hpp"

namespace gv
{
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
	int numEntitiesCreated = 0;
	unsigned int endBeforeResize = Pickups.size();
	Pickups.resize(Pickups.size() + entities.size());
	for (unsigned int i = endBeforeResize; i < Pickups.size(); i++)
	{
		Pickup* newPickup = &Pickups[i];
		newPickup->entity = entities[numEntitiesCreated++];
		newPickups.push_back(newPickup);
	}

	EntityListAddUniqueEntitiesToSuspect(entities, Subscribers);
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
		EntityComponentManager* entityComponentManager = EntityComponentManager::GetSingleton();
		if (entityComponentManager)
		{
			AgentComponentManager* agentComponentManager = static_cast<AgentComponentManager*>(
			    entityComponentManager->GetComponentManagerForType(ComponentType::Agent));

			if (agentComponentManager && agentComponentManager->IsSubscribed(claimer))
			{
				Pickup* pickup = GetPickup(pickupEntity);

				if (!pickup)
					return false;

				Need* needPickupAffects =
				    agentComponentManager->GetAgentNeed(claimer, pickup->AffectsNeed);

				// TODO: For testing only
				if (needPickupAffects)
				{
					needPickupAffects->Level -= 100;
					LOGD << "Entity " << claimer << " restored need "
					     << needPickupAffects->Def->Name << " by 100 picking up " << pickupEntity;
				}

				// TODO: Are we allocating for this? :(
				EntityList entitiesPickedUp;
				entitiesPickedUp.push_back(pickupEntity);
				UnsubscribeEntities(entitiesPickedUp);

				if (pickup->DestroySelfOnPickup)
					entityComponentManager->MarkDestroyEntities(entitiesPickedUp);

				return true;
			}
		}
	}
	return false;
}
}