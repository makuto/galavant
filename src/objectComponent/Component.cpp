#ifndef COMPONENT_CPP
#define COMPONENT_CPP

#include <iostream>

#include "Component.hpp"
#include "ObjectType.hpp"
#include "Object.hpp"

Component::Component()
{

}

Component::~Component()
{

}

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

// Initialize the component. Components should be able to be reused after initialize is called.
// Components should store their parent object's address for later use
// Return false if initialization failed for any reason, and the object creation will
// be aborted
bool Component::initialize(Object* newParentObject)
{
    return false;
}

// Once all sibling components in an object have been initialize, postInitialize is executed.
// Use this function to find all sibling components that your component is dependent on, as well
// as any component-component initialization communication that might need to happen.
bool Component::postInitialize()
{
    return true;
}

// The parent Object has been scheduled to be destroyed. preDestroy() is called on every component
// before components are actually destroyed. This is the "last supper" for components. Use
// this time to do any component-component destruction communication
void Component::preDestroy()
{

}

// The parent Object is being destroyed. Perform any destruction necessary here. It is up to
// you to decide what is destroyed in destroy() and what is cleared in initialize().
void Component::destroy()
{

}

// Perform all logic for your component here
void Component::update(ComponentUpdateParams& updateParams)
{

}

// Output or render as much helpful information as you can about your component in this
// function.
void Component::debugPrint()
{
    std::cout << "WARNING: Undefined debugPrint for component of type " << (int)type << "\n";
}

// Copy all data needed to make an exact replica of the current component to the
// new component. This function will be used when an entire Object or Component is moved.
// Return false if there was an error copying (or it is impossible to copy)
// The new component has already been initialized, but not postInitialized (should it be?)
// You should NOT set the current Component's parentObject pointer to the newComponent's
bool Component::copyComponent(Object* newParentObject, Component* newComponent)
{
    return false;
}
#endif
