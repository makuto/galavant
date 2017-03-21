#pragma once

namespace gv
{
enum class ComponentType : unsigned int
{
	None = 0,

	Test,
	Movement,
	Agent,
	Plan,

	ComponentType_count
};
};