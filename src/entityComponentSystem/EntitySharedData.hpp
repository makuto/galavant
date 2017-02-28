#pragma once

#include "EntityTypes.hpp"
#include "../world/Position.hpp"

namespace gv
{
// TODO: Make it such that things can own the position and this module is used for lookup externally
// only. If nothing owns the position this module will anonymously own it until someone steps up
// This is so I can follow the principle that "That which changes the data, owns the data". It also 
// seems fucking awful to do a map lookup every time anyone wants to read or write a position

// TODO: Make it clear that we expect the given positions to hang around
void EntityCreatePositions(const EntityList& entities, PositionRefList& positions);
void EntityDestroyPositions(const EntityList& entities);

void EntitySetPositions(const EntityList& entities, const PositionList& positions);
void EntitySetPosition(const Entity& entity, const Position& position);

void EntityGetPositions(const EntityList& entities, PositionRefList& positionsOut);
// If an entity doesn't have a position, it will be created at 0, 0, 0
Position* EntityGetPosition(const Entity& entity);
};