#ifndef POOLEDCOMPONENTMANAGER_H__
#define POOLEDCOMPONENTMANAGER_H__

// TODO: Replace FragmentedPool with a better pool
#include "../util/FragmentedPool.hpp"
#include "EntityTypes.hpp"
#include "ComponentManager.hpp"

/* --PooledComponentManager--
PooledComponentManager is a general purpose ComponentManager that assumes you're managing your
components in a standard way.

You should only use PooledComponentManager if it is a good fit for your manager (i.e. you don't need
to do anything special in Subscribe/Unsubscribe etc.) - in other words, use the right tool for the
job. Writing a ComponentManager from scratch is not difficult. Read through the assumptions. If your
use case doesn't match all of those assumptions, you should write a specialized manager instead.

Assumptions
------------
All components have identical data.

Whatever subscribes entities knows all of the data needed to create a Component.

All components are treated the exact same way. For example, if you have a field
GiveMeSpecialTreatment on your component, it would be better to remove that field and instead have
all components desiring special treatment in a SpecialTreatment list. This makes it so you don't
have to have complicated branching deep in your Component update loop to give special treatment. If
this is the case, you probably shouldn't be using PooledComponentManager.
*/

template <class T>
struct Component
{
	Entity entity;
	T data;
};

template <class T>
class PooledComponentManager : public ComponentManager
{
private:
	// TODO: Replace FragmentedPool with a better pool
	FragmentedPool<Component<T> > Components;

public:
	virtual ~PooledComponentManager()
	{
	}

	virtual void SubscribeEntities(Component<T>& components)
	{
	}

	virtual void UnsubscribeEntities(EntityList& entities)
	{
	}
};

#endif /* end of include guard: POOLEDCOMPONENTMANAGER_H__ */