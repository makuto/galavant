#pragma once

#include "../entityComponentSystem/EntityTypes.hpp"
#include "../world/Position.hpp"

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
		Entity,
		Position
	};

	ParamType Type;

	union
	{
		int IntValue;
		float FloatValue;
		bool BoolValue;

		gv::Entity EntityValue;
		gv::Position PositionValue;
	};

	Parameter()
	{
		new(&PositionValue) gv::Position();
	}
};

typedef std::vector<Parameter> ParameterList;
typedef std::vector<Parameter>::iterator ParameterListIterator;
typedef std::vector<Parameter>::const_iterator ParameterListConstIterator;
typedef std::vector<Parameter>::reverse_iterator ParameterListReverseIterator;

struct TaskEvent
{
	enum class TaskResult
	{
		None = 0,

		TaskSucceeded,
		TaskFailed
	};

	TaskResult Result;
	gv::Entity entity;
};
}