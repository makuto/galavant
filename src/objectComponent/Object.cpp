#ifndef OBJECT_CPP
#define OBJECT_CPP

#include <assert.h>

#include "Object.hpp"
#include "Component.hpp"

const int COMPONENTHANDLE_NULL = -1;


Object::Object()
{
    initialize();
}

Object::~Object()
{

}

void Object::initialize()
{
    shouldDestroy = false;
    numActiveComponents = 0;
    id = OBJECT_ID_NONE;
    type = nullptr;
}

ObjectID Object::getObjectID()
{
    return id;
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

void Object::setObjectID(ObjectID newObjectID)
{
    id = newObjectID;
}

void Object::setObjectType(ObjectType* newObjectType)
{
    type = newObjectType;
}

void Object::addComponent(Component* newComponent)
{
    if (numActiveComponents < OBJECT_MAX_COMPONENTS)
    {
        components[numActiveComponents] = newComponent;
        numActiveComponents++;
    }
    else
    {
        // Tried to add more than OBJECT_MAX_COMPONENTS
        assert(0);
    }
}

int Object::getNumActiveComponents()
{
    return numActiveComponents;
}

// Run postInitialize on all components
bool Object::postInitializeComponents()
{
    // For each active component
    for (int i = 0; i < numActiveComponents; i++)
    {
        Component* currentComponent = getComponentAtIndex(i);

        if (currentComponent)
        {
            // Run postInitialize on the component
            if (!currentComponent->postInitialize())
                return false;
        }
    }
    return true;
}

// Run preDestroy on all components
void Object::preDestroyComponents()
{
    // For each active component
    for (int i = 0; i < numActiveComponents; i++)
    {
        Component* currentComponent = getComponentAtIndex(i);

        if (currentComponent)
        {
            currentComponent->preDestroy();
        }
    }
}

// Resets components[] by changing numActiveComponents to 0; this is
// only used if a partially initialized Object's component failed to initialize
void Object::resetComponentsArray()
{
    numActiveComponents = 0;
}
#endif
