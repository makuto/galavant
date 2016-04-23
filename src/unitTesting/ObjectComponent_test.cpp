#include <iostream>
#include <base2.0/dataStructures/pool.hpp>
#include "../objectComponent/ObjectComponentManager.hpp"
#include "../objectComponent/ObjectType.hpp"
#include "../objectComponent/Component.hpp"
#include "../objectComponent/ComponentManager.hpp"

struct ComponentUpdateParams
{
    float timeStep;
};

class TestComponent : public Component
{
    public:
        TestComponent()
        {
            std::cout << "+++ TestComponent constructed!\n";
        }
        virtual ~TestComponent()
        {
            std::cout << "--- TestComponent destructed\n";
        }
        virtual bool initialize(Object* newParentObject)
        {
            parentObject = newParentObject;
            type = ComponentType::TEST;
            std::cout << "Initialized new TestComponent!\n";
            return true;
        }
        virtual bool postInitialize()
        {
            std::cout << "  postInitialized new TestComponent!\n";
            return true;
        }
        virtual void preDestroy()
        {
            std::cout << "  preDestroy TestComponent!\n";
        }
        virtual void destroy()
        {
            std::cout << "destroy TestComponent!\n";
        }
        virtual void update(ComponentUpdateParams& updateParams)
        {
            std::cout << "  updating TestComponent!\n";
        }
        virtual void debugPrint()
        {
            std::cout << "  debugPrint TestComponent! parent obj: " << parentObject << "\n";
        }
        virtual bool copyComponent(Object* newParentObject, Component* newComponent)
        {
            std::cout << "  Copying TestComponent!\n";
            return true;
        }
};


class TestComponentManager : public ComponentManager
{
    private:
        Component* activeComponents[100];
    public:
        TestComponentManager()
        {
            std::cout << "+++ TestComponentManager created\n";
            initialize();
        }
        virtual ~TestComponentManager()
        {
            std::cout << "--- TestComponentManager destructor\n";
        }
        virtual bool initialize()
        {
            std::cout << "TestComponentManager initialized\n";
            for (int i = 0; i < 100; i++)
            {
                activeComponents[i] = nullptr;
            }
            return true;
        }
        virtual void destroy()
        {
            std::cout << "TestComponentManager destroyed\n";
        }
        virtual void updateAllComponents(ComponentUpdateParams& componentUpdateParams)
        {
            std::cout << "  Manager: Updating components\n";
            for (int i = 0; i < 100; i++)
            {
                Component* currentComponent = activeComponents[i];

                if (currentComponent != nullptr)
                {
                    std::cout << "  Manager: Running update on " << currentComponent << "\n";
                    currentComponent->update(componentUpdateParams);
                }
            }
        }
        virtual Component* getNewRawComponent()
        {
            std::cout << "  Manager: Creating component\n";
            Component* newComponent = new TestComponent;

            for (int i = 0; i < 100; i++)
            {
                Component* currentComponent = activeComponents[i];
                if (currentComponent == nullptr)
                {
                    newComponent->localID = i;
                    activeComponents[i] = newComponent;
                    break;
                }
            }

            return newComponent;
        }
        virtual void destroyComponent(Component* component)
        {
            std::cout << "  Manager: Destroying component\n";
            activeComponents[component->localID] = nullptr;
            component->destroy();
            delete component;
        }
};

int main()
{
    std::cout << "Testing ObjectComponent system\n";
    TestComponentManager testComponentManager;
    testComponentManager.initialize();

    ObjectComponentManager objectComponentManager;
    objectComponentManager.addComponentManager(ComponentType::TEST, &testComponentManager);

    ObjectType testObject;
    testObject.components[0] = ComponentType::TEST;
    testObject.totalUsedComponents = 1;

    objectComponentManager.addObjectType(ObjectTypeID::TEST, testObject);

    std::cout << "First object addr: " << objectComponentManager.createObjectOfTypeID(ObjectTypeID::TEST) << "\n";

    {
        ComponentUpdateParams updateParams;

        updateParams.timeStep = 0.016;

        objectComponentManager.updateComponentManagerOfType(ComponentType::TEST, updateParams);

        objectComponentManager.destroyObjectsWithRequestedDestruction();
    }

    std::cout << "Destroying all objects\n";

    objectComponentManager.destroyAllActiveObjects();
    return 1;
}
