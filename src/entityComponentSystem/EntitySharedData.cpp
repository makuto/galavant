#include "EntitySharedData.hpp"

#include <map>
#include <cassert>

namespace gv
{
typedef std::map<Entity, Position> EntityPositionMap;
typedef std::map<Entity, Position*> EntityPositionRefMap;

struct EntitySharedData
{
	// Entity positions with no current owner
	EntityPositionMap UnownedEntityPositions;

	// Entity positions owned by an external module
	EntityPositionRefMap EntityPositionRefs;
};

static EntitySharedData s_Data;

void EntityCreatePositions(const EntityList& entities, PositionRefList& positions)
{
	// Should have same number of positions to entities
	assert(entities.size() == positions.size());

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		Entity currentEntity = entities[i];

		// Ensure we aren't creating positions which have already been created
		{
			EntityPositionRefMap::iterator findPositionRef =
			    s_Data.EntityPositionRefs.find(currentEntity);
			assert(findPositionRef == s_Data.EntityPositionRefs.end());
		}

		// Destroy the unowned position for the entity, if it exists
		{
			EntityPositionMap::iterator findPosition = s_Data.UnownedEntityPositions.find(currentEntity);
			if (findPosition != s_Data.UnownedEntityPositions.end())
				s_Data.UnownedEntityPositions.erase(findPosition);
		}

		s_Data.EntityPositionRefs[currentEntity] = positions[i];
	}
}

void EntityDestroyPositions(const EntityList& entities)
{
	// TODO: This is bad :(
	for (EntityPositionRefMap::iterator it = s_Data.EntityPositionRefs.begin();
	     it != s_Data.EntityPositionRefs.end();)
	{
		bool foundEntity = false;
		for (Entity currentEntity : entities)
		{
			if (it->first == currentEntity)
			{
				it = s_Data.EntityPositionRefs.erase(it);
				foundEntity = true;
				break;
			}
		}

		if (!foundEntity)
			++it;
	}
}

void EntitySetPositions(const EntityList& entities, const PositionList& positions)
{
	// Should have same number of positions to entities
	assert(entities.size() == positions.size());

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		Entity currentEntity = entities[i];
		Position currentPosition = positions[i];
		EntitySetPosition(currentEntity, currentPosition);
	}
}

// This sets the value of the position, not the reference (use EntityCreatePosition for that)
void EntitySetPosition(const Entity& entity, const Position& position)
{
	EntityPositionRefMap::iterator findRef = s_Data.EntityPositionRefs.find(entity);
	if (findRef != s_Data.EntityPositionRefs.end())
		(*findRef->second) = position;
	else
		s_Data.UnownedEntityPositions[entity] = position;
}

void EntityGetPositions(const EntityList& entities, PositionRefList& positionsOut)
{
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		Entity currentEntity = entities[i];
		positionsOut[i] = EntityGetPosition(currentEntity);
	}
}

// If an entity doesn't have a position, it will be created at 0, 0, 0
Position* EntityGetPosition(const Entity& entity)
{
	// Check if we have a reference to the entity's position (another module owns it)
	EntityPositionRefMap::iterator findRef = s_Data.EntityPositionRefs.find(entity);
	if (findRef != s_Data.EntityPositionRefs.end())
		return findRef->second;

	// Check if we own the position
	EntityPositionMap::iterator findUnowned = s_Data.UnownedEntityPositions.find(entity);
	if (findUnowned == s_Data.UnownedEntityPositions.end())
	{
		// We don't own the position; create it
		Position zeroPosition;
		s_Data.UnownedEntityPositions[entity] = zeroPosition;
		findUnowned = s_Data.UnownedEntityPositions.find(entity);
	}

	return &findUnowned->second;
}
}