#include "ComponentManager.hpp"
#include "util/Logging.hpp"

namespace gv
{
ComponentManager::~ComponentManager()
{
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

			LOGD << "Manager " << (int)Type << " unsubscribed " << entities.size() << " entities";
		}
	}
}

bool ComponentManager::IsSubscribed(Entity entity)
{
	return EntityListFindEntity(Subscribers, entity);
}

ComponentType ComponentManager::GetType()
{
	return Type;
}
}