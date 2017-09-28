#include "ComponentManager.hpp"
#include "entityComponentSystem/EntityComponentManager.hpp"
#include "util/Logging.hpp"

namespace gv
{
ComponentManager::ComponentManager()
{
	EntityComponentManager::AddComponentManager(this);
}

ComponentManager::ComponentManager(const char* debugName) : DebugName(debugName)
{
	EntityComponentManager::AddComponentManager(this);
}

ComponentManager::~ComponentManager()
{
	EntityComponentManager::RemoveComponentManager(this);
}

void ComponentManager::UnsubscribeEntitiesInternal(const EntityList& entities)
{
	// Classes don't have to implement this function if all they want is a subscriber list
}

void ComponentManager::UnsubscribeEntities(const EntityList& entities)
{
	if (!entities.empty())
	{
		// Copy for modification
		EntityList entitiesToUnsubscribe;
		EntityListAppendList(entitiesToUnsubscribe, entities);

		// Make sure they're actually subscribed
		EntityListRemoveUniqueEntitiesInSuspect(Subscribers, entitiesToUnsubscribe);

		if (!entitiesToUnsubscribe.empty())
		{
			UnsubscribeEntitiesInternal(entitiesToUnsubscribe);

			// Remove from subscribers
			EntityListRemoveNonUniqueEntitiesInSuspect(entitiesToUnsubscribe, Subscribers);

			if (DebugName)
				LOGD << "Manager " << DebugName << " unsubscribed " << entitiesToUnsubscribe.size()
				     << " entities";
			else
				LOGD << "Manager (UNNAMED) unsubscribed " << entitiesToUnsubscribe.size()
				     << " entities";
		}
	}
}

bool ComponentManager::IsSubscribed(Entity entity)
{
	return EntityListFindEntity(Subscribers, entity);
}
}