#pragma once

#include "../../entityComponentSystem/EntityTypes.hpp"
#include "../../world/Position.hpp"

namespace gv
{
/* --MovementManager--
This was needed so that I could have a heavily Unreal-specific class (TestMovementComponent) without
needing to compromise the Galavant lib. I'll probably think of a better way to do this eventually
*/
class MovementManager
{
public:
	virtual ~MovementManager() = default;
	virtual void PathEntitiesTo(const EntityList& entities, const PositionList& positions) = 0;
};
}