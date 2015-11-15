#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "ObjectID.hpp"
#include "ObjectType.hpp"

enum ComponentType;
struct ComponentUpdateParams;
class Object;

/* --Component--
 * Components are modular bits of logic and data designed to work with other
 * modular components. Objects are collections of components. Component is an abstract
 * class.
*/
class Component
{
    public:
        // Note that the constructor and destructor shouldn't have to be executed
        // every time a Component is created. Initialize and Deinitalize should do everything
        // necessary for a component to be reused
        virtual ~Component();

        // Getters for types
        ComponentType getType();
        ObjectType getOwnerType();
        ObjectID getOwnerID();

        // Initialize the component. Components should be able to be reused after initialize is called.
        // Components should store their parent object's address for later use
        // Return false if initialization failed for any reason, and the object creation will
        // be aborted
        virtual bool initialize(Object* parentObject);

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

        // TODO: Component save and load functions
        //virtual bool save(SaveBuffer *saveBuffer);
        //virtual bool load(LoadBuffer *loadBuffer);

    protected:
    private:
        ComponentType       type;
        ObjectType          ownerType;
        ObjectID            ownerID;
};

#endif // COMPONENT_HPP
