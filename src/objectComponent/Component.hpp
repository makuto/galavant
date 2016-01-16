#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "ObjectID.hpp"
#include "ObjectType.hpp"

/* --Component--
 * Components are modular bits of logic and data designed to work with other
 * modular components. Objects are collections of components. Component is an abstract
 * class.
*/
enum ComponentType;
struct ComponentUpdateParams;
class Object;
class ComponentManager;
class Component
{
    public:
        Component();

        // Note that the constructor and destructor shouldn't have to be executed
        // every time a Component is created. Initialize and Deinitalize should do everything
        // necessary for a component to be reused
        virtual ~Component();

        // Getters for types
        ComponentType getType();
        ObjectType getOwnerType();
        ObjectID getOwnerID();

        // This function should NEVER return NULL on a valid initialized Component.
        // This function is used by ComponentManagers when moving Components in memory
        // Make SURE you set your parentObject pointer in initialize()!
        Object* getParentObject();

        // Initialize the component. Components should be able to be reused after initialize is called.
        // Components should store their parent object's address for later use
        // Return false if initialization failed for any reason, and the object creation will
        // be aborted
        virtual bool initialize(Object* newParentObject);

        // Once all sibling components in an object have been initialize, postInitialize is executed.
        // Use this function to find all sibling components that your component is dependent on, as well
        // as any component-component initialization communication that might need to happen.
        virtual bool postInitialize();

        // The parent Object has been scheduled to be destroyed. preDestroy() is called on every component
        // before components are actually destroyed. This is the "last supper" for components. Use
        // this time to do any component-component destruction communication
        virtual void preDestroy();

        // The parent Object is being destroyed. Perform any destruction necessary here. It is up to
        // you to decide what is destroyed in destroy() and what is cleared in initialize().
        virtual void destroy();

        // Perform all logic for your component here
        virtual void update(ComponentUpdateParams& updateParams);

        // Output or render as much helpful information as you can about your component in this
        // function.
        virtual void debugPrint();

        // Copy all data needed to make an exact replica of the current component to the
        // new component. This function will be used when an entire Object or Component is moved.
        // Return false if there was an error copying (or it is impossible to copy)
        // The new component has already been initialized, but not postInitialized (should it be?)
        // You should NOT set the current Component's parentObject pointer to the newComponent's
        virtual bool copyComponent(Object* newParentObject, Component* newComponent);

        // TODO: Component save and load functions
        //virtual bool save(SaveBuffer *saveBuffer);
        //virtual bool load(LoadBuffer *loadBuffer);

        // This value can be used by the ComponentManager to find a component
        // in a Pool, array, list, etc
        int                 localID;
    protected:

        // The parent object of this component. This value should always be
        // valid and set as the ComponentManager will use it
        Object*             parentObject;

        ComponentType       type;
    private:
        ObjectType          ownerType;
        ObjectID            ownerID;
};

#endif // COMPONENT_HPP
