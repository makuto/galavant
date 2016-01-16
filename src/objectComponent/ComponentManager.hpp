#ifndef COMPONENTMANAGER_HPP
#define COMPONENTMANAGER_HPP

#include "ComponentTypes.hpp"

/* --ComponentManager--
 * ComponentManagers handle the storage, creation, and updating of a single
 * type of Component. ComponentManager is a polymorphic class so that
 * unique behaviors can be determined for all of these functions.
 * */

class Object;
class Component;
struct ComponentUpdateParams;
class ComponentManager
{
    public:
        ComponentManager();

        virtual ~ComponentManager();

        ComponentType getType();

        // Initialize the ComponentManager. The manager should work without
        // the constructor having been run before
        virtual bool initialize();

        // Do any destruction necessary for this manager.
        virtual void destroy();

        // Update all of the active Components, passing them the ComponentUpdateParams
        virtual void updateAllComponents(ComponentUpdateParams& componentUpdateParams);

        // Return a pointer to a fresh Component. Do not run initialize() or
        // anything on the component - this function's caller should handle that.
        // Return NULL if the request cannot be completed (e.g., the pool is full)
        virtual Component* getNewRawComponent();

        // Run destroy() on the Component and remove the component from this manager's memory.
        virtual void destroyComponent(Component* component);

    private:
        ComponentType type;
};
#endif

