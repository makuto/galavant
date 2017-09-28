#pragma once

#include "EntityTypes.hpp"

namespace gv
{
// Base class for all ComponentManagers. Note that it is intended to be a very minimal interface
// because the Entity Component System is designed to be minimally polymorphic. This is so it is
// more obvious when things happen. It also reduces the amount of boilerplate needed when creating a
// new ComponentManager.
class ComponentManager
{
protected:
	const char* DebugName;

	EntityList Subscribers;

	virtual void UnsubscribeEntitiesInternal(const EntityList& entities);

public:
	ComponentManager();
	ComponentManager(const char* debugName);
	virtual ~ComponentManager();

	// Calls UnsubscribeEntitiesInternal on entities actually subscribed (filters out
	// non-subscribers), then once you're done it removes the entities from the Subscribers list
	void UnsubscribeEntities(const EntityList& entities);

	bool IsSubscribed(Entity entity);
};
};