#include "EntityComponentManager.hpp"

#include "util/Logging.hpp"

#include <cassert>

namespace gv
{
Entity EntityComponentManager::NextNewEntity = 1;
ComponentManagerList EntityComponentManager::ComponentManagers;
EntityComponentManager g_EntityComponentManager;

EntityComponentManager::EntityComponentManager()
{
}

EntityComponentManager::~EntityComponentManager()
{
	DestroyAllEntities();
}

void EntityComponentManager::AddComponentManager(ComponentManager *manager)
{
	if (!manager)
		return;

	ComponentManagers.push_back(manager);
}

void EntityComponentManager::RemoveComponentManager(ComponentManager *manager)
{
	for (ComponentManagerListIterator it = ComponentManagers.begin(); it != ComponentManagers.end();)
	{
		if ((*it) == manager)
		{
			it = ComponentManagers.erase(it);
		}
		else
			++it;
	}
}

void EntityComponentManager::GetNewEntities(EntityList &list, int count)
{
	for (int i = 0; i < count; i++)
	{
		list.push_back(NextNewEntity);
		ActiveEntities.push_back(NextNewEntity);
		NextNewEntity++;
	}
}

// Mark an Entity for destruction. It is not destroyed immediately; rather, it is destroyed when
// DestroyEntitiesPendingDestruction() is called.
void EntityComponentManager::MarkDestroyEntities(EntityList &entities)
{
	EntitiesPendingDestruction.insert(EntitiesPendingDestruction.end(), entities.begin(),
	                                  entities.end());
}

void EntityComponentManager::UnsubscribeEntitiesFromAllManagers(EntityList &entitiesToUnsubscribe)
{
	// Unsubscribe all of the entities from all ComponentManagers
	// Some component managers will not actually have the Entity being destroyed subscribed, but
	// that's fine
	for (ComponentManager *currentComponentManager : ComponentManagers)
	{
		currentComponentManager->UnsubscribeEntities(entitiesToUnsubscribe);
	}
}

// Destroy all entities which have been marked for destruction. Because an entity is just an ID
// and a collection of components, this function must notify all ComponentManagers that the
// Entity should be unsubscribed from their services.
void EntityComponentManager::DestroyEntitiesPendingDestruction()
{
	if (!EntitiesPendingDestruction.empty())
	{
		// Ensure there are no duplicates in EntitiesPendingDestruction
		EntityListSortAndRemoveDuplicates(EntitiesPendingDestruction);

		UnsubscribeEntitiesFromAllManagers(EntitiesPendingDestruction);

		// Remove all destroyed entities from the ActiveEntities list
		EntityListRemoveNonUniqueEntitiesInSuspect(EntitiesPendingDestruction, ActiveEntities);

		EntitiesPendingDestruction.clear();
	}
}

// Destroys all entities that were created by this EntityComponentManager (i.e. all entities in
// the ActiveEntities list)
void EntityComponentManager::DestroyAllEntities()
{
	// Mark all active entities as pending destruction. I do this because if I just destroyed
	// ActiveEntities, then EntitiesPendingDestruction could have entities which have already been
	// destroyed and entities which weren't active (i.e. not created by this manager) but marked for
	// destruction. By effectively combining the two in MarkDestroyEntities(), we ensure all
	// entities are going to be destroyed
	MarkDestroyEntities(ActiveEntities);
	DestroyEntitiesPendingDestruction();

	ActiveEntities.clear();  // this should be empty anyways
	EntitiesPendingDestruction.clear();
}
}