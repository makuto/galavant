#include "InteractComponentManager.hpp"

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

void InteractComponentManager::CreatePickups(int count, PickupRefList& newPickups)
{
	unsigned int endBeforeResize = Pickups.size();
	Pickups.resize(Pickups.size() + count);
	for (unsigned int i = endBeforeResize; i < Pickups.size(); i++)
		newPickups.push_back(&Pickups[i]);
}
}