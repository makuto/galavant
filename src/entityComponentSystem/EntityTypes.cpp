#include <algorithm>

#include "EntityTypes.hpp"

#include "util/CppHelperMacros.hpp"

namespace gv
{
const Entity NullEntity = 0;

bool EntityComparator(Entity a, Entity b)
{
	return a < b;
}

void EntityListAppendList(EntityList& list, const EntityList& listToAdd)
{
	list.insert(list.end(), listToAdd.begin(), listToAdd.end());
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
			it = list.erase(it);
			continue;
		}

		if (lastEntity != currentEntity)
			lastEntity = currentEntity;

		++it;
	}
}

void EntityListAddUniqueEntitiesToSuspect(const EntityList& list, EntityList& suspectList)
{
	for (EntityListConstIterator it = list.begin(); it != list.end(); ++it)
	{
		Entity currentEntity = (*it);

		if (!EntityListFindEntity(suspectList, currentEntity))
			suspectList.push_back(currentEntity);
	}
}

// Remove all entities from suspectList which are already in list
void EntityListRemoveNonUniqueEntitiesInSuspect(const EntityList& list, EntityList& suspectList)
{
	for (EntityListConstIterator it = list.begin(); it != list.end(); ++it)
	{
		const Entity currentEntity = (*it);
		for (EntityListIterator sIt = suspectList.begin(); sIt != suspectList.end();)
		{
			Entity suspectEntity = (*sIt);

			if (suspectEntity == currentEntity)
			{
				sIt = suspectList.erase(sIt);

				// don't assume there's only one non-unique; suspectList could have duplicates
				// break;
			}
			else
				++sIt;
		}
	}
}

// Remove all entities from suspectList which are not already in list
void EntityListRemoveUniqueEntitiesInSuspect(const EntityList& list, EntityList& suspectList)
{
	for (EntityListIterator it = suspectList.begin(); it != suspectList.end();)
	{
		bool found = false;
		Entity suspectEntity = (*it);
		for (EntityListConstIterator sIt = list.begin(); sIt != list.end();)
		{
			const Entity currentEntity = (*sIt);

			if (currentEntity == suspectEntity)
			{
				found = true;
				break;
			}
			else
				++sIt;
		}

		if (found)
			it++;
		else
			it = suspectList.erase(it);
	}
}

bool EntityListFindEntity(const EntityList& list, Entity entity)
{
	return IS_IN_LIST(list, entity);
}
}