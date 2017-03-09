#include "WorldState.hpp"

#include "../entityComponentSystem/EntitySharedData.hpp"

namespace gv
{
WorldState& WorldStateManager::GetWorldStateForAgent(Entity agent)
{
	EntityWorldStateMap::iterator findEnt = EntityWorldStates.find(agent);

	if (findEnt == EntityWorldStates.end())
	{
		WorldState worldState;
		worldState.SourceAgent.SourceEntity = agent;
		EntityWorldStates[agent] = worldState;
		findEnt = EntityWorldStates.find(agent);
	}

	WorldState& worldState = findEnt->second;

	// Positions are likely enough to change that we'll just always update it
	worldState.SourceAgent.position = (*EntityGetPosition(agent));

	return worldState;
}
}