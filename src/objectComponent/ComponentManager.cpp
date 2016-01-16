#ifndef COMPONENTMANAGER_CPP
#define COMPONENTMANAGER_CPP

#include "ComponentManager.hpp"

ComponentType ComponentManager::getType()
{
    return type;
}

ComponentManager::ComponentManager()
{

}

ComponentManager::~ComponentManager()
{

}

// Initialize the ComponentManager. The manager should work without
// the constructor having been run before
bool ComponentManager::initialize()
{
    return true;
}

// Do any destruction necessary for this manager.
void ComponentManager::destroy()
{

}

// Update all of the active Components, passing them the ComponentUpdateParams
void ComponentManager::updateAllComponents(ComponentUpdateParams& componentUpdateParams)
{

}

// Return a pointer to a fresh Component. Do not run initialize() or
// anything on the component - this function's caller should handle that.
// Return NULL if the request cannot be completed (e.g., the pool is full)
Component* ComponentManager::getNewRawComponent()
{
    return nullptr;
}

// Run destroy() on the Component and remove the component from this manager's memory.
void ComponentManager::destroyComponent(Component* component)
{

}

#endif
