#pragma once

#include "../entityComponentSystem/EntityTypes.hpp"

namespace Htn
{
// Htn::Parameter is used as a way to send arbitrary arguments between different types of Tasks
// TODO: Add getters/setters so this is safe to use
struct Parameter
{
	enum class ParamType
	{
		None = 0,

		// Primitives
		Int,
		Float,
		Bool,

		// Game-specific
		Entity
	};

	ParamType Type;

	union
	{
		int IntValue;
		float FloatValue;
		bool BoolValue;

		Entity EntityValue;
	};
};

typedef std::vector<Parameter> ParameterList;
typedef std::vector<Parameter>::iterator ParameterListIterator;
typedef std::vector<Parameter>::const_iterator ParameterListConstIterator;
typedef std::vector<Parameter>::reverse_iterator ParameterListReverseIterator;

// The arguments passed to most all Task functions
struct TaskArguments
{
	Entity Agent;
	// AgentState* AgentState;
	// WorldState* WorldState;
	ParameterList Parameters;
};
}