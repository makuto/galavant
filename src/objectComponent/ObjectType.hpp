#ifndef OBJECTTYPE_HPP
#define OBJECTTYPE_HPP

#include "ComponentTypes.hpp"

#define OBJECT_MAX_COMPONENTS 10

struct ObjectType
{
    // An array of ComponentTypes that this ObjectType requires
    ComponentType       components[OBJECT_MAX_COMPONENTS];

    // The number of Components this Object will actually use
    int                 totalUsedComponents;
};

#endif
