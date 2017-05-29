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
	Interact,

	ComponentType_count
};
};