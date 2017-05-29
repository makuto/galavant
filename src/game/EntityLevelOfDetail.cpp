#include "game/EntityLevelOfDetail.hpp"
#include "entityComponentSystem/EntitySharedData.hpp"

namespace gv
{
namespace EntityLOD
{
EntityLODSettings g_EntityLODSettings;

bool ShouldRenderForPlayer(Position& position)
{
	const EntityPlayerSharedData& playerData = EntityPlayerGetSharedData();
	const Position* playerPosition = playerData.PlayerPosition;
	if (playerPosition)
	{
		return (playerPosition->ManhattanTo(position) <= g_EntityLODSettings.PlayerManhattanViewDistance);
	}
	else
		// When there's no player nothing should be rendered
		return false;
}
}
}