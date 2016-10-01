#include <iostream>
#include <cassert>

#include "../entityComponentSystem/EntityTypes.hpp"
#include "../entityComponentSystem/EntityComponentManager.hpp"
#include "../entityComponentSystem/ComponentManager.hpp"
#include "../entityComponentSystem/PooledComponentManager.hpp"

void TestEntityLists(void)
{
	EntityList testList = {1, 4, 2, 6, 3, 2, 1244};

	std::cout << "Testing EntityList...\n";

	EntityListSortAndRemoveDuplicates(testList);

	for (EntityListIterator it = testList.begin(); it != testList.end(); ++it)
	{
		std::cout << "\t" << (*it) << "\n";
	}

	std::cout << "Done\n\n";
}

void TestEntityCreationAndDestruction(void)
{
	class TestComponentManager : public ComponentManager
	{
	private:
		EntityList Subscribers;

	public:
		TestComponentManager(void)
		{
			Type = 1;
		}
		virtual ~TestComponentManager()
		{
			// On the off chance that the EntityComponentManager's destructor is called after this
			// Manager's, make sure this manager still unsubscribes everything.
			if (!Subscribers.empty())
			{
				EntityList unsubscribeList;
				unsubscribeList.insert(unsubscribeList.end(), Subscribers.begin(),
				                       Subscribers.end());
				UnsubscribeEntities(unsubscribeList);
			}
		};

		void SubscribeEntities(const EntityList& entities)
		{
			EntityList entitiesToSubscribe;
			entitiesToSubscribe.insert(entitiesToSubscribe.begin(), entities.begin(),
			                           entities.end());
			// Ensure that we don't resubscribe any entities by removing anything in entities which
			// is already in Subscribers
			EntityListRemoveNonUniqueEntitiesInSuspect(Subscribers, entitiesToSubscribe);

			for (EntityListIterator it = entitiesToSubscribe.begin();
			     it != entitiesToSubscribe.end(); ++it)
			{
				Entity currentEntity = (*it);

				std::cout << "\t\t+ Subscribing " << currentEntity << "\n";

				Subscribers.push_back(currentEntity);
			}

			EntityListSortAndRemoveDuplicates(Subscribers);
		}

		virtual void UnsubscribeEntities(const EntityList& entities)
		{
			if (!entities.empty() && !Subscribers.empty())
			{
				for (EntityListConstIterator it = entities.begin(); it != entities.end(); ++it)
				{
					const Entity entityToRemove = (*it);

					for (EntityListIterator sIt = Subscribers.begin(); sIt != Subscribers.end();)
					{
						Entity currentEntity = (*sIt);
						if (currentEntity == entityToRemove)
						{
							std::cout << "\t\t- Unsubscribing " << currentEntity << "\n";
							Subscribers.erase(sIt);
							break;
						}
						else
							++sIt;
					}
				}
			}
		}

		virtual void Update(float ms)
		{
			std::cout << "\tUpdating TestComponentManager...\n";

			for (EntityListIterator it = Subscribers.begin(); it != Subscribers.end(); ++it)
			{
				Entity currentEntity = (*it);
				std::cout << "\t\t" << currentEntity << "\n";
			}

			std::cout << "\tDone\n";
		}
	};

	EntityComponentManager entityComponentManager;
	TestComponentManager testComponentManager;

	std::cout << "Testing Entity Creation and Destruction...\n";

	entityComponentManager.AddComponentManagerOfType(testComponentManager.GetType(),
	                                                 &testComponentManager);

	EntityList newEntities;

	entityComponentManager.GetNewEntities(newEntities, 10);

	testComponentManager.SubscribeEntities(newEntities);

	// Simulate a game loop (poorly)
	EntityList createList;
	for (int i = 0; i < 100; i++)
	{
		testComponentManager.Update(i);

		// Do some weird creation/destruction of entities
		if (rand() % 4 && !createList.empty())
		{
			EntityList destroyList;
			destroyList.push_back(createList[i]);

			entityComponentManager.MarkDestroyEntities(destroyList);
		}
		else
		{
			// Note that I'm resubscribing entities here. The ComponentManager needs to know how to
			// handle this (maybe by using EntityListRemoveNonUniqueEntitiesInSuspect() etc.)
			entityComponentManager.GetNewEntities(createList, rand() % 10);
			testComponentManager.SubscribeEntities(createList);
		}

		entityComponentManager.DestroyEntitiesPendingDestruction();
	}

	std::cout << "Done\n\n";
}

void TestEntityComponentTypes(void)
{
	struct TestComponent
	{
		int value;
	};
	class TestPooledComponentManager : public PooledComponentManager<TestComponent>
	{
	public:
		TestPooledComponentManager() : PooledComponentManager<TestComponent>(1000)
		{
		}
		virtual ~TestPooledComponentManager()
		{
		}
		virtual void Update(float ms)
		{
			std::cout << "\tUpdating TestPooledComponentManager...\n";
			PooledComponentManager::FragmentedPoolIterator it =
			    PooledComponentManager::NULL_POOL_ITERATOR;
			for (PooledComponent<TestComponent>* currentComponent = ActivePoolBegin(it);
			     currentComponent != nullptr && it != PooledComponentManager::NULL_POOL_ITERATOR;
			     currentComponent = GetNextActivePooledComponent(it))
			{
				std::cout << "\t\t" << currentComponent->entity
				          << " value: " << currentComponent->data.value << "\n";
			}
			std::cout << "\tDone\n";
		}
	};

	std::cout << "\nTesting Entity Component Types...\n";

	EntityList newEntities;
	EntityComponentManager entityComponentManager;
	TestPooledComponentManager testComponentManager;

	entityComponentManager.GetNewEntities(newEntities, 10);

	// Simulate a game loop (poorly)
	EntityList createList;
	for (int i = 0; i < 100; i++)
	{
		testComponentManager.Update(i);

		// Do some weird creation/destruction of entities
		if (rand() % 4 && !createList.empty())
		{
			EntityList destroyList;
			destroyList.push_back(createList[i]);

			entityComponentManager.MarkDestroyEntities(destroyList);
		}
		else
		{
			std::vector<PooledComponent<TestComponent> > newComponents;

			// Note that I'm resubscribing entities here. The ComponentManager needs to know how to
			// handle this (maybe by using EntityListRemoveNonUniqueEntitiesInSuspect() etc.)
			entityComponentManager.GetNewEntities(createList, rand() % 10);

			newComponents.resize(createList.size());

			int i = 0;
			for (EntityListIterator it = createList.begin(); it != createList.end(); ++it)
			{
				PooledComponent<TestComponent>* newComponent = &newComponents[i++];
				newComponent->entity = (*it);
				newComponent->data.value = rand() % 10000;
			}
			std::cout << "\tAttempting to subscribe " << newComponents.size()
			          << " components (some duplicates)\n";
			testComponentManager.SubscribeEntities(newComponents);
		}

		entityComponentManager.DestroyEntitiesPendingDestruction();
	}
	std::cout << "Done\n\n";
}

int main()
{
	TestEntityLists();
	TestEntityCreationAndDestruction();
	TestEntityComponentTypes();

	return 1;
}