#pragma once

#include "world/Position.hpp"

namespace gv
{
namespace EntityLOD
{
struct EntityLODSettings
{
	float PlayerManhattanViewDistance;
};

extern EntityLODSettings g_EntityLODSettings;

bool ShouldRenderForPlayer(Position& position);
}
}