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

#endif
