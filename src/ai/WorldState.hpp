#pragma once

#include <map>

#include "../world/Position.hpp"
#include "../entityComponentSystem/EntityTypes.hpp"

namespace gv
{
struct AgentState
{
	Entity SourceEntity;
	Position position;
	Position TargetPosition;
};

/* --WorldState--
WorldState represents a mutable, copyable reference to all AI-relevant data in the World.
	Mutable: The data can be manipulated freely without repurcussion. Note that changing data in 
	 WorldState is NOT expected to actually change the world - WorldState is like a mirror world
	Copyable: The data can be copied without a significant performance impact. This means that in
	 order to support mutability, things like changelists might need to be implemented for large 
	 datasets instead of actually copying the dataset
*/
struct WorldState
{
	// Because an agent is almost always going to...well, maybe this shouldn't be here. For now it
	//  will stay.
	AgentState SourceAgent;

	// This will be removed eventually
	int TestStateChange;
};

class WorldStateManager
{
private:
	typedef std::map<Entity, WorldState> EntityWorldStateMap;
	EntityWorldStateMap EntityWorldStates;
public:
	WorldState& GetWorldStateForAgent(Entity agent);
};
};