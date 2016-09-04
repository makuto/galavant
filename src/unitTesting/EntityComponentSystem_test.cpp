#include <iostream>
#include <cassert>

#include "../entityComponentSystem/EntityTypes.hpp"
#include "../entityComponentSystem/EntityComponentManager.hpp"
#include "../entityComponentSystem/ComponentManager.hpp"

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

	TestComponentManager testComponentManager;
	EntityComponentManager entityComponentManager;

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

int main()
{
	TestEntityLists();
	TestEntityCreationAndDestruction();

	return 1;
}