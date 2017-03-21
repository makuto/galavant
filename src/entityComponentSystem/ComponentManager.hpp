#pragma once

#include "EntityTypes.hpp"
#include "ComponentTypes.hpp"

namespace gv
{
// Base class for all ComponentManagers. Note that it is intended to be a very minimal interface
// because the Entity Component System is designed to be minimally polymorphic. This is so it is
// more obvious when things happen. It also reduces the amount of boilerplate needed when creating a
// new ComponentManager.
class ComponentManager
{
protected:
	// You should set this type in your constructor
	ComponentType Type;

public:
	virtual ~ComponentManager();

	virtual void UnsubscribeEntities(const EntityList& entities);

	ComponentType GetType();
};

};