#ifndef ENTITYCOMPONENTMANAGER_H__
#define ENTITYCOMPONENTMANAGER_H__

#include <map>

#include "EntityTypes.hpp"
#include "ComponentManager.hpp"

/* --EntityComponentManager--
EntityComponentManager is intended to be a very minimal managing class for the Entity Component
System. Its primary task is to facilitate the creation and destruction of Entities.

Note that EntityComponentManager does not track nor handle subscribing of entities to
ComponentManagers - that is handled directly by ComponentManagers. This means that when an Entity is
destroyed, the EntityComponentManager must send the entire list of all unsubscribing entities to
every ComponentManager which is registered with this ECM. This is less than optimal, but
permissable.
*/
class EntityComponentManager
{
private:
	typedef std::map<ComponentType, ComponentManager *> ComponentManagerMap;
	typedef ComponentManagerMap::iterator ComponentManagerMapIterator;

	ComponentManagerMap ComponentManagers;

	EntityList ActiveEntities;
	EntityList EntitiesPendingDestruction;

	// This is static so that if there are multiple EntityComponentManagers, every Entity is
	// guaranteed to be unique globally
	static Entity NextNewEntity;

	// This is a private helper method; use MarkDestroyEntities() and
	// DestroyEntitiesPendingDestruction() externally
	void UnsubscribeEntitiesFromAllManagers(EntityList &entitiesToUnsubscribe);

public:
	EntityComponentManager();
	~EntityComponentManager();

	// Sets the ComponentManager for a ComponentType. Returns false if there is already a manager
	// for that type (it will not be set)
	bool AddComponentManagerOfType(ComponentType type, ComponentManager *manager);

	bool AddComponentManager(ComponentManager *manager);

	// Returns the ComponentManager assigned to the provided type, or nullptr if there isn't one
	// assigned. If your ComponentManager needs another, it is preferable to get its dependencies
	// directly (i.e. passed in during a initialize() function) rather than using this function
	ComponentManager *GetComponentManagerForType(ComponentType type);

	// Creates the given number of entities, adds them to the ActiveEntities list, and appends them
	// to the provided list
	void GetNewEntities(EntityList &list, int count);

	// Mark Entities for destruction. They are not destroyed immediately; rather, they is destroyed
	// when DestroyEntitiesPendingDestruction() is called.
	void MarkDestroyEntities(EntityList &entities);

	// Destroy all entities which have been marked for destruction. Because an entity is just an ID
	// and a collection of components, this function must notify all ComponentManagers that the
	// Entity should be unsubscribed from their services.
	void DestroyEntitiesPendingDestruction(void);

	// Destroys all entities that were created by this EntityComponentManager (i.e. all entities in
	// the ActiveEntities list)
	void DestroyAllEntities(void);
};

#endif /* end of include guard: ENTITYCOMPONENTMANAGER_H__ */