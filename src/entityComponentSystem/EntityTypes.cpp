#include <algorithm>

#include "EntityTypes.hpp"

const Entity NullEntity = 0;

bool EntityComparator(Entity a, Entity b)
{
	return a < b;
}

void EntityListSort(EntityList& list)
{
	std::sort(list.begin(), list.end(), EntityComparator);
}

void EntityListSortAndRemoveDuplicates(EntityList& list)
{
	Entity lastEntity = NullEntity;

	EntityListSort(list);

	// This could probably use a rewrite
	for (EntityListIterator it = list.begin(); it != list.end();)
	{
		Entity currentEntity = (*it);

		if (lastEntity != NullEntity && currentEntity == lastEntity)
		{
			list.erase(it);
			continue;
		}

		if (lastEntity != currentEntity)
			lastEntity = currentEntity;

		++it;
	}
}

// Remove all entities from suspectList which are already in list
void EntityListRemoveNonUniqueEntitiesInSuspect(EntityList& list, EntityList& suspectList)
{
	for (EntityListIterator it = list.begin(); it != list.end(); ++it)
	{
		Entity currentEntity = (*it);
		for (EntityListIterator sIt = suspectList.begin(); sIt != suspectList.end();)
		{
			Entity suspectEntity = (*sIt);

			if (suspectEntity == currentEntity)
			{
				suspectList.erase(sIt);

				// don't assume there's only one non-unique; suspectList could have duplicates
				// break;
			}
			else
				++sIt;
		}
	}
}