#ifndef COMPONENT_CPP
#define COMPONENT_CPP

#include "Component.hpp"
#include "ObjectType.hpp"
#include "Object.hpp"

ComponentType Component::getType()
{
    return type;
}
ObjectType Component::getOwnerType()
{
    return ownerType;
}
ObjectID Component::getOwnerID()
{
    return ownerID;
}

// This function should NEVER return NULL on a valid initialized Component.
// This function is used by ComponentManagers when moving Components in memory
// Make SURE you set your parentObject pointer in initialize()!
Object* Component::getParentObject()
{
    return parentObject;
}

#endif
