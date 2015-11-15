#ifndef OBJECT_CPP
#define OBJECT_CPP

#include "Object.hpp"
#include "Component.hpp"

const int COMPONENTHANDLE_NULL = -1;


Object::Object()
{
    shouldDestroy = false;
    numActiveComponents = 0;
    id = OBJECT_ID_NONE;
    type = nullptr;
}
Object::~Object()
{

}

// Provides a layer of abstraction around the internal storage of Component pointers
Component* Object::getComponentAtIndex(int i)
{
    if (i >= 0 && i < numActiveComponents)
        return components[i];
    return nullptr;
}

// Searches through components this object is composed of for one of
// the provided type. Returns a handle
// Returns a ComponentHandle which can be used to get a pointer to
// a component of the requested type.
Object::ComponentHandle Object::getComponentHandleForType(ComponentType componentType)
{
    for (int i = 0; i < numActiveComponents; i++)
    {
        Component* currentComponent = getComponentAtIndex(i);

        if (currentComponent)
        {
            if (currentComponent->getType() == componentType)
                return i;
        }
    }

    return COMPONENTHANDLE_NULL;
}

// Returns true if the ComponentHandle is valid, and points to a valid
// Component.
bool Object::isValidComponentHandle(ComponentHandle componentHandle)
{
    if (componentHandle == Object::COMPONENTHANDLE_NULL
        || componentHandle >= numActiveComponents)
        return false;
    return true;
}

// Returns a pointer to the component indicated by the handle.
// This pointer should NOT be stored on the heap! Components can
// move around in memory, so storing direct pointers is not ok!
Component* Object::getComponent(Object::ComponentHandle componentHandle)
{
    if (isValidComponentHandle(componentHandle))
        return getComponentAtIndex(componentHandle);
    return nullptr;
}

// Returns the value of shouldDestroy. If true, whatever is managing
// the object should notify all components of the impending destruction,
// destroy the components, then destroy the object
bool Object::shouldDestroyObject()
{
    return shouldDestroy;
}

// Sets the value of shouldDestroy to true. Once set, it cannot be
// undone. The actual destruction should be handled by the object's
// manager after checking shouldDestroyObject()
// This function is meant to be used by the object's components to self-destruct
// Destruction will not happen immediately - it is up to the object's manager
void Object::requestObjectDestruction()
{
    shouldDestroy = true;
}
#endif
