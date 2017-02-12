#pragma once

#include "../world/Position.hpp"

namespace gv
{
struct AgentState
{
	Position position;
};

/* --WorldState--
WorldState represents a mutable, copyable reference to all AI-relevant data in the World.
	Mutable: The data can be manipulated freely without repurcussion. Note that changing data in 
	 WorldState is NOT expected to actually change the world - WorldState is like a mirror world
	Copyable: The data can be copied without a significant amount of time. This means that in order
	 to support mutability, things like changelists might need to be implemented for large datasets
	 instead of actually copying the dataset
*/
struct WorldState
{
	// Because an agent is almost always going to...well, maybe this shouldn't be here. For now it
	//  will stay.
	AgentState SourceAgent;
};
};