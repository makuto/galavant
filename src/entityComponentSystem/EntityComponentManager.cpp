#include "EntityComponentManager.hpp"

#include "util/Logging.hpp"

#include <cassert>

namespace gv
{
Entity EntityComponentManager::NextNewEntity = 1;
EntityComponentManager *EntityComponentManager::Singleton = nullptr;

EntityComponentManager::EntityComponentManager()
{
	// Should not create more than one ECM! Commented due to Unreal's hotreloading :(
	// assert(!Singleton);
	Singleton = this;
}

EntityComponentManager::~EntityComponentManager()
{
	DestroyAllEntities();

	Singleton = nullptr;
}

// Sets the ComponentManager for a ComponentType. Returns false if there is already a manager
// for that type (it will not be set)
bool EntityComponentManager::AddComponentManagerOfType(ComponentType type,
                                                       ComponentManager *manager)
{
	if (manager)
	{
		EntityComponentManager::ComponentManagerMapIterator findIt = ComponentManagers.find(type);

		// Make sure there isn't already a ComponentManager for the type
		assert(findIt == ComponentManagers.end());

		ComponentManagers[type] = manager;

		if (type != manager->GetType())
			LOGW << "ComponentManager of type " << (int)manager->GetType()
			     << " registered for type " << (int)type;
	}

	return false;
}

bool EntityComponentManager::AddComponentManager(ComponentManager *manager)
{
	if (!manager)
		return false;

	if (manager->GetType() == ComponentType::None)
		LOGE << "Registered ComponentManager " << manager << " with no Type!";

	return AddComponentManagerOfType(manager->GetType(), manager);
}

// Returns the ComponentManager assigned to the provided type, or nullptr if there isn't one
// assigned. If your ComponentManager needs another, it is preferable to get its dependencies
// directly (i.e. passed in during a initialize() function)
ComponentManager *EntityComponentManager::GetComponentManagerForType(ComponentType type)
{
	EntityComponentManager::ComponentManagerMapIterator findIt = ComponentManagers.find(type);
	if (findIt == ComponentManagers.end())
		return nullptr;

	return findIt->second;
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
	for (EntityComponentManager::ComponentManagerMapIterator it = ComponentManagers.begin();
	     it != ComponentManagers.end(); ++it)
	{
		ComponentManager *currentComponentManager = it->second;

		if (currentComponentManager)
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

EntityComponentManager *EntityComponentManager::GetSingleton()
{
	// If failed, someone is requesting ECM before one has been initialized! Commented due to
	// Unreal's hotreloading :(
	// assert(Singleton);
	return Singleton;
}
}