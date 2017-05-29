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

	EntityPlayerSharedData PlayerData;
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
			EntityPositionMap::iterator findPosition =
			    s_Data.UnownedEntityPositions.find(currentEntity);
			if (findPosition != s_Data.UnownedEntityPositions.end())
				s_Data.UnownedEntityPositions.erase(findPosition);
		}

		s_Data.EntityPositionRefs[currentEntity] = positions[i];
	}
}

void EntityDestroyPositions(const EntityList& entities)
{
	// TODO: This is a bit bad: loop through all positions for each entity to remove approaches N
	// squared
	for (EntityPositionRefMap::iterator it = s_Data.EntityPositionRefs.begin();
	     it != s_Data.EntityPositionRefs.end();)
	{
		bool foundEntity = false;
		for (Entity currentEntity : entities)
		{
			if (it->first == currentEntity)
			{
				// Copy to owned, in case other things still care about position
				// TODO: Should that be the case? How to handle destroyed entities?
				if (it->second)
					s_Data.UnownedEntityPositions[it->first] = *it->second;

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

const EntityPlayerSharedData& EntityPlayerGetSharedData()
{
	return s_Data.PlayerData;
}

void EntityPlayerRegisterPosition(const Entity entity, Position* position)
{
	if (s_Data.PlayerData.PlayerEntity)
	{
		LOGE << "A Player Entity " << s_Data.PlayerData.PlayerEntity
		     << " has already been registered (tried to register " << entity << ")!";
		return;
	}

	s_Data.PlayerData.PlayerEntity = entity;
	s_Data.PlayerData.PlayerPosition = position;

	// Add to owned lists as well
	gv::EntityList playerEntities = {entity};
	gv::PositionRefList playerPositionList = {position};
	gv::EntityCreatePositions(playerEntities, playerPositionList);
}
void EntityPlayerUnregisterPosition()
{
	if (s_Data.PlayerData.PlayerEntity)
	{
		gv::EntityList playerEntities = {s_Data.PlayerData.PlayerEntity};
		gv::EntityDestroyPositions(playerEntities);
		s_Data.PlayerData.PlayerEntity = 0;
		s_Data.PlayerData.PlayerPosition = nullptr;
	}
}
}