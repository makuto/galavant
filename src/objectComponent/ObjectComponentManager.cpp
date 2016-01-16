#ifndef OBJECTCOMPONENTMANAGER_CPP
#define OBJECTCOMPONENTMANAGER_CPP

#include <assert.h>
#include <iostream>

#include "ObjectComponentManager.hpp"

#include "ComponentManager.hpp"
#include "Component.hpp"

ObjectComponentManager::ObjectComponentManager():objectPool(ObjectComponentManager::OBJECT_POOL_SIZE)
{

}

ObjectComponentManager::~ObjectComponentManager()
{
    destroyAllActiveObjects();
}

// Destroys all active objects
void ObjectComponentManager::destroyAllActiveObjects()
{
    destroyObjectsWithRequestedDestructionInternal(true);
}

// Adds a ComponentManager to the ComponentManagerDictionary. If
// there is already a manager for the specified type, it will be overwritten
void ObjectComponentManager::addComponentManager(ComponentType managerComponentType, ComponentManager* newManager)
{
    addComponentManagerToDictionary(managerComponentType, newManager);
}

// Adds an ObjectType of newObjectTypeID to the ObjectType dictionary.
// If there is already an ObjectType of the specified ObjectTypeID, it will
// be overwritten. This is the function where ObjectTypeIDs are tied to
// a specific ObjectType
void ObjectComponentManager::addObjectType(ObjectTypeID newObjectTypeID, ObjectType newObjectType)
{
    addObjectTypeToDictionary(newObjectTypeID, newObjectType);
}

// Create a new Object of the specified type. Returns NULL if
// the ObjectType couldn't be found, the object pool is full, or
// Object failed to initialize
// TODO: IMPORTANT! Do extensive testing on this function, making sure things
//      are being deleted when they should be if initialization fails for any reason
Object* ObjectComponentManager::createObjectOfTypeID(ObjectTypeID newObjectTypeID)
{
    // Get the new Object's ObjectType
    ObjectType* newObjectType = getObjectTypeByID(newObjectTypeID);

    // Couldn't find the requested ObjectType
    if (!newObjectType)
        return nullptr;

    // Create the Object
    Object* newObject = createNewEmptyObject();

    if (newObject)
    {
        // Set the Object's ObjectType
        newObject->setObjectType(newObjectType);

        // Add components to the new Object
        if (createAndAddComponentsForObject(newObject, newObjectType))
        {
            // Run postInitialize on the object's components
            if (postInitializeObject(newObject))
                return newObject;
        }
    }

    // Object creation failed; remove the Object
    removeEmptyObject(newObject);
    return nullptr;
}

bool ObjectComponentManager::addComponentOfTypeToObject(Object* object, ComponentType componentToAdd)
{
    // Find the ComponentManager for the specified type
    ComponentManager* componentManager = getComponentManagerForType(componentToAdd);

    if (!componentManager)
    {
        // ComponentManager couldn't be found
        return false;
    }

    // Get a fresh new component from the manager
    Component* newComponent = componentManager->getNewRawComponent();

    if (!newComponent)
    {
        // Failed to get new component from manager
        return false;
    }

    // Initialize the component
    if (!newComponent->initialize(object))
    {
        // Component failed to initialize
        return false;
    }

    object->addComponent(newComponent);

    return true;
}

bool ObjectComponentManager::createAndAddComponentsForObject(Object* object, ObjectType* newObjectType)
{
    bool allComponentsSuccessfullyCreated = true;

    if (!object || !newObjectType)
        return false;

    // Add components one by one according to the ObjectType spec
    for (int i = 0; i < newObjectType->totalUsedComponents; i++)
    {
        ComponentType currentComponentType = newObjectType->components[i];

        if (!addComponentOfTypeToObject(object, currentComponentType))
        {
            allComponentsSuccessfullyCreated = false;
            break;
        }
    }

    // Make sure the Object has all of its needed components
    if (object->getNumActiveComponents() != newObjectType->totalUsedComponents)
    {
        allComponentsSuccessfullyCreated = false;
    }

    if (!allComponentsSuccessfullyCreated)
    {
        // Something went wrong during Component creation - destroy all other components on the object
        // and return failure
        destroyAllComponentsOnObject(object);
        return false;
    }

    return true;
}

// Tells the ComponentManager of each component on an object to destroy the component
// Make sure to run preDestroy on all of the components before calling this function
// (it is not called within the function because this function is used when Objects
// might not be fully formed (like in createAndAddComponentsForObject) or haven't been
// postInitialized)
void ObjectComponentManager::destroyAllComponentsOnObject(Object* object)
{
    for (int i = 0; i < object->getNumActiveComponents(); i++)
    {
        Component* currentComponent = object->getComponentAtIndex(i);

        if (!currentComponent)
            continue;

        ComponentType currentComponentType = currentComponent->getType();

        if (currentComponentType != ComponentType::NONE)
        {
            ComponentManager* currentComponentManager = getComponentManagerForType(currentComponentType);

            if (currentComponentManager)
            {
                currentComponentManager->destroyComponent(currentComponent);
            }
        }
    }
}

// Run postInitialize() on all of the Object's components
bool ObjectComponentManager::postInitializeObject(Object* newObject)
{
    if (newObject)
    {
        return newObject->postInitializeComponents();
    }

    return false;
}

void ObjectComponentManager::preDestroyObject(Object* object)
{
    if (object)
    {
        object->preDestroyComponents();
    }
}

// Update the ComponentManager of the specified type
// There is no updateAll() function because it is expected that there
// will be a specific order the ComponentManagers should be updated.
// Having updateComponentManagerOfType allows that order to be explicitly determined
void ObjectComponentManager::updateComponentManagerOfType(ComponentType componentType, ComponentUpdateParams& componentUpdateParams)
{
    ComponentManager* componentManager = getComponentManagerForType(componentType);

    if (componentManager)
    {
        componentManager->updateAllComponents(componentUpdateParams);
    }
    else
    {
        // Tried to update a component manager which doesn't exist
        assert(0);
    }
}

// Destroy all Objects which have shouldDestroy set to true
// This function should be called once per frame/after updating all ComponentManagers
// TODO: This is currently looping through all objects. May need to optimize later
void ObjectComponentManager::destroyObjectsWithRequestedDestruction()
{
    destroyObjectsWithRequestedDestructionInternal(false);
}

void ObjectComponentManager::destroyObjectsWithRequestedDestructionInternal(bool forceDestroyAllObjects)
{
    std::vector<ObjectPoolHandle> objectsToDestroy;

    // Build a list of all Objects that have impending destruction
    // A separate list must be used because any other list could have
    // data moving due to the destruction
    for (ActiveObjectsDictionary::iterator it = activeObjects.begin(); it != activeObjects.end(); ++it)
    {
        ObjectPoolHandle currentObjectHandle = it->second;

        if (currentObjectHandle)
        {
            Object* currentObject = &currentObjectHandle->data;

            if (currentObject->shouldDestroyObject() || forceDestroyAllObjects)
                objectsToDestroy.push_back(currentObjectHandle);
        }
    }

    // Destroy all Objects in the destroy list
    for (std::vector<ObjectPoolHandle>::iterator it = objectsToDestroy.begin(); it != objectsToDestroy.end(); ++it)
    {
        if (!(*it))
            continue;

        Object* currentObject = &(*it)->data;

        // Have the Object tell its components that they are going to be destroyed
        preDestroyObject(currentObject);

        // Destroy the object's components
        destroyAllComponentsOnObject(currentObject);

        // Remove the now empty object from the Active Objects dictionary and the Object Pool
        removeEmptyObject(currentObject);
    }
}

/////////////////////////////////////////////////////////////////
// Abstraction layer for ObjectComponentManager data structures
/////////////////////////////////////////////////////////////////

// Creates a new Object, assigns its ID, and adds it to the Active Objects dictionary
// This function does not run initialize() etc
// Make sure to run removeObject() once finished with the object
Object* ObjectComponentManager::createNewEmptyObject()
{
    ObjectPoolHandle newPooledObject = objectPool.getNewData();

    if (newPooledObject)
    {
        Object* newObject = &newPooledObject->data;

        // Initialize the object
        newObject->initialize();

        // Set the new object's ID
        ObjectID newObjectID = generateUniqueObjectID();
        newObject->setObjectID(newObjectID);

        // Make sure the ID is unique
        assert(getObjectByID(newObjectID) == nullptr);

        // Add the Object to the Active Objects Dictionary
        activeObjects[newObjectID] = newPooledObject;

        return newObject;
    }

    // Pool is full!
    std::cout << "WARNING: Object Pool is FULL!\n";
    return nullptr;
}

Object* ObjectComponentManager::getObjectByID(ObjectID objectID)
{
    ActiveObjectsDictionary::iterator findIt = activeObjects.find(objectID);

    if (findIt != activeObjects.end())
    {
        return &findIt->second->data;
    }

    return nullptr;
}

// Removes an object from the ActiveObjects Dictionary and frees it from the object pool.
// This function DOES NOT remove an object's components (hence the "Empty")
// This function should NOT be called if you are iterating over activeObjects, because
// it removes values from activeObjects.
void ObjectComponentManager::removeEmptyObject(Object* objectToRemove)
{
    if (objectToRemove)
    {
        // Find the Object in the activeObjects dictionary
        ActiveObjectsDictionary::iterator findIt = activeObjects.find(objectToRemove->getObjectID());

        if (findIt != activeObjects.end())
        {
            ObjectPoolHandle pooledObject = findIt->second;

            if (pooledObject)
            {
                // Free the object in the pool
                objectPool.removeData(pooledObject);

                // Remove the Object from hte Active Objects dictionary
                activeObjects.erase(findIt);
            }
        }
        else
        {
            // Tried to remove an Object which isn't in this ObjectComponentManager's ActiveObjects
            assert(0);
        }
    }
}

ComponentManager* ObjectComponentManager::getComponentManagerForType(ComponentType componentType)
{
    ComponentManagerDictionary::iterator findIt = componentManagers.find(componentType);

    if (findIt != componentManagers.end())
    {
        return findIt->second;
    }

    return nullptr;
}

void ObjectComponentManager::addComponentManagerToDictionary(ComponentType componentType, ComponentManager* newManager)
{
    componentManagers[componentType] = newManager;
}

ObjectType* ObjectComponentManager::getObjectTypeByID(ObjectTypeID objectTypeID)
{
    ObjectTypeDictionary::iterator findIt = objectTypes.find(objectTypeID);

    if (findIt != objectTypes.end())
    {
        return &findIt->second;
    }

    return nullptr;
}

void ObjectComponentManager::addObjectTypeToDictionary(ObjectTypeID objectTypeID, ObjectType newObjectType)
{
    objectTypes[objectTypeID] = newObjectType;
}

// TODO: Have a more explicit way to get runtime-unique object ids
static ObjectID currentAvailableObjectID = OBJECT_ID_FIRST;
ObjectID ObjectComponentManager::generateUniqueObjectID()
{
    return currentAvailableObjectID++;
}
#endif
