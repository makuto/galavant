#include "ComponentManager.hpp"

namespace gv
{
ComponentManager::~ComponentManager()
{
}

void ComponentManager::UnsubscribeEntities(const EntityList& entities)
{
}

ComponentType ComponentManager::GetType()
{
	return Type;
}
}