#pragma once

#include "entityComponentSystem/EntityTypes.hpp"
#include "world/Position.hpp"

#include <vector>

namespace gv
{
/* --MovementManager--
This was needed so that I could have a heavily Unreal-specific class (UnrealMovementComponent)
without needing to compromise the Galavant lib. I'll probably think of a better way to do this
eventually
*/
class MovementManager
{
public:
	virtual ~MovementManager() = default;
	virtual void PathEntitiesTo(const EntityList& entities, const PositionList& positions) = 0;
	virtual void SetEntitySpeeds(const EntityList& entities, const std::vector<float>& speeds) = 0;
};
}