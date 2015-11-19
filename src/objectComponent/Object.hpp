#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "ObjectID.hpp"
#include "ObjectType.hpp"
#include "ComponentTypes.hpp"

/* --Object--
 * Objects hold a collection of Components. Object provides an abstraction
 * layer for Components so that components can be moved in memory without
 * affecting other Components.
 *
 * Note that Objects cannot hold more than one Component of the same type.
 * If this is required for some reason, you should redesign the system, or
 * make the component support it internally
*/
class Component;
struct ObjectType;
class ObjectComponentManager;
class Object
{
    public:
        Object();
        ~Object();

        // Initialize the object
        // Objects should work completely after initialize, even if the constructor isn't called
        void initialize();

        ObjectID getObjectID();

        // A handle to a Component this Object has
        // Components (and all other external users of an Object's components) should
        // NEVER store pointers to an Object's components on anything other than
        // the stack. ComponentHandles, however, are safe to store. This is so
        // that Components can be moved around in memory without breaking references
        typedef int ComponentHandle;

        // Searches through components this object is composed of for one of
        // the provided type.
        // Returns a ComponentHandle which can be used to get a pointer to
        // a component of the requested type.
        // Note that Objects can only have one Component of the same type
        // Check your ComponentHandle's validity with isValidComponentHandle()
        ComponentHandle getComponentHandleForType(ComponentType componentType);

        // Returns true if the ComponentHandle is valid, and points to a valid
        // Component.
        bool isValidComponentHandle(ComponentHandle componentHandle);

        // Returns a pointer to the component indicated by the handle.
        // This pointer should NOT be stored on the heap! Components can
        // move around in memory, so storing direct pointers is not ok!
        // Note that getComponent() checks the validity of the ComponentHandle
        // Returns nullptr if the Handle is invalid
        Component* getComponent(ComponentHandle componentHandle);

        // Returns the value of shouldDestroy. If true, whatever is managing
        // the object should notify all components of the impending destruction,
        // destroy the components, then destroy the object
        bool shouldDestroyObject();

        // Sets the value of shouldDestroy to true. Once set, it cannot be
        // undone. The actual destruction should be handled by the object's
        // manager after checking shouldDestroyObject()
        // This function is meant to be used by the object's components to self-destruct
        // Destruction will not happen immediately - it is up to the object's manager
        void requestObjectDestruction();

    friend class ObjectComponentManager;
    protected:
        // Functions for ObjectComponentManager
        void setObjectID(ObjectID newObjectID);
        void setObjectType(ObjectType* newObjectType);

        // Add a new, initialized component to this object
        void addComponent(Component* newComponent);

        int getNumActiveComponents();

        // Run postInitialize on all components
        bool postInitializeComponents();

        // Run preDestroy on all components
        void preDestroyComponents();

        // Provides a layer of abstraction around the internal storage of Component pointers
        Component* getComponentAtIndex(int i);

        // Resets components[] by changing numActiveComponents to 0; this is
        // only used if a partially initialized Object's component failed to initialize
        void resetComponentsArray();

    private:
        // A list of pointers to Components the Object currently has (up to numActiveComponents)
        // OBJECT_MAX_COMPONENTS is defined in ObjectType.hpp
        Component*      components[OBJECT_MAX_COMPONENTS];
        int             numActiveComponents;

        ObjectType*     type;
        ObjectID        id;

        // shouldDestroy indicates whether or not the Object should be
        // destroyed. This is meant to be used by Components to self-destruct.
        // shouldDestroy should be checked by whatever is managing
        // the object after the object's components have been updated
        bool            shouldDestroy;

        // The NULL value for ComponentHandles
        static const int COMPONENTHANDLE_NULL = -1;
};

#endif
