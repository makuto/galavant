#pragma once

// TODO: Handle full pool better (will be done when FragmentedPool is replaced)
#include <cassert>
#include <vector>

// TODO: Replace FragmentedPool with a better pool
#include "../util/FragmentedPool.hpp"
#include "EntityTypes.hpp"
#include "ComponentManager.hpp"

namespace gv
{
/* --PooledComponentManager--
PooledComponentManager is a general purpose PooledComponentManager that assumes you're managing your
components in a standard way.

You should only use PooledComponentManager if it is a good fit for your manager (i.e. you don't need
to do anything special in Subscribe/Unsubscribe etc.) - in other words, use the right tool for the
job. Writing a PooledComponentManager from scratch is not difficult. Read through the assumptions.
If your
use case doesn't match all of those assumptions, you should write a specialized manager instead.

Assumptions
------------
All components have identical data.

Whatever subscribes entities knows all of the data needed to create a PooledComponent.

All components are treated the exact same way. For example, if you have a field
GiveMeSpecialTreatment on your component, it would be better to remove that field and instead have
all components desiring special treatment in a SpecialTreatment list. This makes it so you don't
have to have complicated branching deep in your PooledComponent update loop to give special
treatment. If
this is the case, you probably shouldn't be using PooledComponentManager.

Whatever your data is can be copied via the assignment operator '='
*/

template <class T>
struct PooledComponent
{
	Entity entity;
	T data;

	void operator=(const PooledComponent<T>& component)
	{
		entity = component.entity;
		data = component.data;
	}
};

template <class T>
class PooledComponentManager : public ComponentManager
{
private:
	// This list is used only to quickly check whether an entity is subscribed; data should actually
	// be stored in PooledComponents
	EntityList Subscribers;

	// TODO: Replace FragmentedPool with a better pool
	FragmentedPool<PooledComponent<T>> PooledComponents;

protected:
	typedef int FragmentedPoolIterator;
	const int NULL_POOL_ITERATOR = -1;

	// Return the index of the first active data, or NULL_POOL_ITERATOR if the pool is inactive
	PooledComponent<T>* ActivePoolBegin(FragmentedPoolIterator& it)
	{
		for (int i = 0; i < PooledComponents.GetPoolSize(); i++)
		{
			FragmentedPoolData<PooledComponent<T>>* currentPooledComponent =
			    PooledComponents.GetActiveDataAtIndex(i);

			if (currentPooledComponent)
			{
				it = i;
				return &currentPooledComponent->data;
			}
		}

		it = NULL_POOL_ITERATOR;
		return nullptr;
	}

	// Increments iterator until an active component is found.
	// Returns nullptr at end of list
	PooledComponent<T>* GetNextActivePooledComponent(FragmentedPoolIterator& it)
	{
		it++;
		for (; it < PooledComponents.GetPoolSize(); it++)
		{
			FragmentedPoolData<PooledComponent<T>>* currentPooledComponent =
			    PooledComponents.GetActiveDataAtIndex(it);

			if (currentPooledComponent)
				return &currentPooledComponent->data;
		}

		it = NULL_POOL_ITERATOR;
		return nullptr;
	}

	PooledComponent<T>* GetComponent(FragmentedPoolIterator& it)
	{
		FragmentedPoolData<PooledComponent<T>>* pooledComponent =
		    PooledComponents.GetActiveDataAtIndex(it);
		if (pooledComponent)
			return &pooledComponent->data;
		return nullptr;
	}

	// Do whatever your custom manager does for subscribing here.
	// The components are already in the pool.
	// It is safe to subscribe and unsubscribe components in this function
	virtual void SubscribeEntitiesInternal(const EntityList& subscribers,
	                                       std::vector<PooledComponent<T>*>& components)
	{
	}

	// Do whatever your custom manager does for unsubscribing here.
	// The components are still in the subscription list and pool
	// It is safe to subscribe and unsubscribe components in this function
	virtual void UnsubscribeEntitiesInternal(const EntityList& unsubscribers,
	                                         std::vector<PooledComponent<T>*>& components)
	{
	}

	const EntityList& GetSubscriberList() const
	{
		return Subscribers;
	}

public:
	PooledComponentManager(int poolSize) : PooledComponents(poolSize)
	{
	}

	virtual ~PooledComponentManager()
	{
		Reset();
	}

	// If the entity is already subscribed, the input component will be tossed out
	void SubscribeEntities(const std::vector<PooledComponent<T>>& components)
	{
		std::vector<PooledComponent<T>*> newSubscriberComponents(components.size());
		EntityList newSubscribers;

		for (typename std::vector<PooledComponent<T>>::const_iterator it = components.begin();
		     it != components.end(); ++it)
		{
			const PooledComponent<T> currentPooledComponent = (*it);

			// Make sure the Entity isn't already subscribed
			if (EntityListFindEntity(Subscribers, currentPooledComponent.entity))
				continue;

			FragmentedPoolData<PooledComponent<T>>* newPooledComponent =
			    PooledComponents.GetNewData();

			// Pool is full!
			// TODO: handle this elegantly
			assert(newPooledComponent);

			// TODO: Remove allocation by the caller by letting them pull straight from the pool
			newPooledComponent->data = currentPooledComponent;

			newSubscribers.push_back(currentPooledComponent.entity);

			newSubscriberComponents.push_back(&newPooledComponent->data);
		}

		Subscribers.insert(Subscribers.end(), newSubscribers.begin(), newSubscribers.end());

		SubscribeEntitiesInternal(newSubscribers, newSubscriberComponents);
	}

	virtual void UnsubscribeEntities(const EntityList& entities)
	{
		EntityList entitiesToUnsubscribe;
		EntityListAppendList(entitiesToUnsubscribe, entities);

		// Ensure that we only unsubscribe entities which are actually Subscribers
		EntityListRemoveUniqueEntitiesInSuspect(Subscribers, entitiesToUnsubscribe);

		std::vector<PooledComponent<T>*> unsubscribers(entitiesToUnsubscribe.size());

		// Build the list of components we are going to be unsubscribing for the
		//  child of this class. We can probably make this one loop, but we'll save that for later
		for (EntityListConstIterator it = entitiesToUnsubscribe.begin();
		     it != entitiesToUnsubscribe.end(); ++it)
		{
			Entity currentEntity = (*it);

			for (int i = 0; i < PooledComponents.GetPoolSize(); i++)
			{
				FragmentedPoolData<PooledComponent<T>>* currentPooledComponent =
				    PooledComponents.GetActiveDataAtIndex(i);

				if (currentPooledComponent && currentPooledComponent->data.entity == currentEntity)
					unsubscribers.push_back(&currentPooledComponent->data);
			}
		}

		// Let child do whatever it needs to unsubscribe the given entities
		// Note that the child can actually unsubscribe entities in their function. This will
		// currently only mean we might try to double-unsubscribe in the code below, which is not
		// bad
		UnsubscribeEntitiesInternal(entitiesToUnsubscribe, unsubscribers);

		// Remove the entities from pool (freeing memory)
		for (EntityListConstIterator it = entitiesToUnsubscribe.begin();
		     it != entitiesToUnsubscribe.end(); ++it)
		{
			Entity currentEntity = (*it);

			for (int i = 0; i < PooledComponents.GetPoolSize(); i++)
			{
				FragmentedPoolData<PooledComponent<T>>* currentPooledComponent =
				    PooledComponents.GetActiveDataAtIndex(i);

				if (currentPooledComponent && currentPooledComponent->data.entity == currentEntity)
					PooledComponents.RemoveData(currentPooledComponent);
			}
		}

		// Remove all entities which were unsubscribed from the Subscribers list
		EntityListRemoveNonUniqueEntitiesInSuspect(entitiesToUnsubscribe, Subscribers);
	}

	void Reset()
	{
		PooledComponents.Clear();
		Subscribers.clear();
	}
};
}