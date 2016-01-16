#ifndef OBJECTCOMPONENTMANAGER_HPP
#define OBJECTCOMPONENTMANAGER_HPP

#include <base2.0/dataStructures/pool.hpp>
#include <map>

#include "Object.hpp"
#include "ObjectType.hpp"
#include "ComponentTypes.hpp"
#include "ObjectID.hpp"

/* --ObjectComponentManager--
 * ObjectComponentManager manages ComponentManagers and Objects. It
 * performs the necessary interactions between these two systems
 * */
class ComponentManager;
struct ComponentUpdateParams;
class ObjectComponentManager
{
    public:
        ObjectComponentManager();
        ~ObjectComponentManager();

        // Adds a ComponentManager to the ComponentManagerDictionary. If
        // there is already a manager for the specified type, it will be overwritten
        void addComponentManager(ComponentType managerComponentType, ComponentManager* newManager);

        // Adds an ObjectType of newObjectTypeID to the ObjectType dictionary.
        // If there is already an ObjectType of the specified ObjectTypeID, it will
        // be overwritten. This is the function where ObjectTypeIDs are tied to
        // a specific ObjectType
        void addObjectType(ObjectTypeID newObjectTypeID, ObjectType newObjectType);

        // Create a new Object of the specified type. Returns NULL if
        // the ObjectType couldn't be found, the object pool is full, or
        // Object failed to initalize
        Object* createObjectOfTypeID(ObjectTypeID newObjectTypeID);

        // Update the ComponentManager of the specified type
        // There is no updateAll() function because it is expected that there
        // will be a specific order the ComponentManagers should be updated.
        // Having updateComponentManagerOfType allows that order to be explicitly determined
        void updateComponentManagerOfType(ComponentType componentType, ComponentUpdateParams& componentUpdateParams);

        // Destroys all active objects
        void destroyAllActiveObjects();

        // Destroy all Objects which have shouldDestroy set to true
        // This function should be called once per frame/after updating all ComponentManagers
        void destroyObjectsWithRequestedDestruction();

    private:
        // The structure that holds all ComponentManagers
        typedef std::map<ComponentType, ComponentManager*> ComponentManagerDictionary;
        ComponentManagerDictionary componentManagers;

        // The pooled structure that holds all Objects
        typedef Pool<Object> ObjectPool;

        static const int OBJECT_POOL_SIZE = 100;
        ObjectPool objectPool;

        // A handle to a pooled object
        typedef PoolData<Object>* ObjectPoolHandle;

        // The dictionary which holds Objects associated with their IDs
        typedef std::map<ObjectID, ObjectPoolHandle> ActiveObjectsDictionary;
        ActiveObjectsDictionary activeObjects;

        // The structure that holds all ObjectTypes
        typedef std::map<ObjectTypeID, ObjectType> ObjectTypeDictionary;
        ObjectTypeDictionary objectTypes;

        // Abstraction layer for ObjectComponentManager data structures

        // Creates a new Object, assigns its ID, and adds it to the Active Objects dictionary
        // This function does not run initialize() etc
        // Make sure to run removeObject() once finished with the object
        Object* createNewEmptyObject();

        Object* getObjectByID(ObjectID objectID);

        // Removes an object from the ActiveObjects Dictionary and frees it from the object pool.
        // This function DOES NOT remove an object's components (hence the "Empty")
        // This function should NOT be called if you are iterating over activeObjects, because
        // it removes values from activeObjects. Do not call this function if you are iterating
        // over active objects in the object pool either
        void removeEmptyObject(Object* objectToRemove);

        ComponentManager* getComponentManagerForType(ComponentType componentType);

        void addComponentManagerToDictionary(ComponentType componentType, ComponentManager* newManager);

        ObjectType* getObjectTypeByID(ObjectTypeID objectTypeID);

        void addObjectTypeToDictionary(ObjectTypeID objectTypeID, ObjectType newObjectType);

        // Generates a globally unique ObjectID (runtime only; these aren't persisted)
        ObjectID generateUniqueObjectID();

        bool addComponentOfTypeToObject(Object* object, ComponentType componentToAdd);

        bool createAndAddComponentsForObject(Object* newObject, ObjectType* newObjectType);

        void destroyObjectsWithRequestedDestructionInternal(bool forceDestroyAllObjects);

        // Tells the ComponentManager of each component on an object to destroy the component
        // Make sure to run preDestroy on all of the components before calling this function
        // (it is not called within the function because this function is used when Objects
        // might not be fully formed (like in createAndAddComponentsForObject) or haven't been
        // postInitialized)
        void destroyAllComponentsOnObject(Object* newObject);

        // Run postInitialize() on all of the Object's components
        bool postInitializeObject(Object* newObject);

        // Run preDestroy() on all of the Object's components
        void preDestroyObject(Object* object);
};
#endif
