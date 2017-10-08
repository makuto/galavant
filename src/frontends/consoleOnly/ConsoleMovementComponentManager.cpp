#include "ConsoleMovementComponentManager.hpp"

#include <algorithm>

ConsoleMovementComponentManager g_ConsoleMovementComponentManager;

ConsoleMovementComponentManager::ConsoleMovementComponentManager()
{
	DebugName = "ConsoleMovementComponentManager";
}

void ConsoleMovementComponentManager::Initialize(
    gv::CallbackContainer<Htn::TaskEventCallback>* taskEventCallbacks)
{
	TaskEventCallbacks = taskEventCallbacks;
}

void ConsoleMovementComponentManager::UnsubscribeEntitiesInternal(const gv::EntityList& entities)
{
	for (MovementComponentList::iterator it = MovementComponents.begin();
	     it != MovementComponents.end();)
	{
		gv::Entity currentEntity = (*it).entity;
		bool foundEntity = false;
		for (const gv::Entity& unsubscribeEntity : entities)
		{
			if (currentEntity == unsubscribeEntity)
			{
				it = MovementComponents.erase(it);
				foundEntity = true;
				break;
			}
		}

		if (!foundEntity)
			++it;
	}
}

void ConsoleMovementComponentManager::CreateMovementComponents(
    const gv::EntityList& entities, MovementComponentRefList& newMovementComponents)
{
	// Only create MovementComponents which aren't already subscribed
	gv::EntityList entitiesToSubscribe = entities;
	gv::EntityListRemoveNonUniqueEntitiesInSuspect(Subscribers, entitiesToSubscribe);

	unsigned int endBeforeResize = MovementComponents.size();
	MovementComponents.resize(MovementComponents.size() + entitiesToSubscribe.size());

	int numEntitiesCreated = 0;
	for (unsigned int i = endBeforeResize; i < MovementComponents.size(); i++)
	{
		MovementComponent* newMovementComponent = &MovementComponents[i];
		newMovementComponent->entity = entitiesToSubscribe[numEntitiesCreated++];
		newMovementComponents.push_back(newMovementComponent);
	}

	// We've already made sure all entities in the list are unique
	gv::EntityListAppendList(Subscribers, entitiesToSubscribe);
}

void ConsoleMovementComponentManager::Update(float deltaSeconds)
{
	Htn::TaskEventList eventList;
	gv::EntityList entitiesToUnsubscribe;

	for (MovementComponent& component : MovementComponents)
	{
		gv::Position targetPosition;

		// Decide where we're going to go
		{
			if (component.GoalWorldPosition)
			{
				if (component.Position.ManhattanTo(component.GoalWorldPosition) >
				    component.GoalManDistanceTolerance)
				{
					targetPosition = component.GoalWorldPosition;
				}
				else
				{
					component.GoalWorldPosition.Reset();

					Htn::TaskEvent goalPositionReachedEvent{
					    Htn::TaskEvent::TaskResult::TaskSucceeded, component.entity};
					eventList.push_back(goalPositionReachedEvent);
				}
			}
		}

		if (component.ResourceType != gv::WorldResourceType::None &&
		    !component.ResourcePosition.Equals(component.Position, 100.f))
		{
			// Give ResourceLocator our new position
			gv::WorldResourceLocator::MoveResource(component.ResourceType, component.entity,
			                                       component.ResourcePosition, component.Position);
			component.ResourcePosition = component.Position;
		}

		// Perform movement
		if (targetPosition)
		{
			gv::Position deltaPosition = targetPosition - component.Position;
			gv::Position deltaVelocity = deltaPosition.GetSafeNormal(0.1f);
			deltaVelocity.Scale(component.MaxSpeed * deltaSeconds);
			// Disallow flying
			deltaVelocity[2] = 0;

			component.Position += deltaVelocity;
		}
	}

	if (TaskEventCallbacks && !eventList.empty())
	{
		for (gv::CallbackCall<Htn::TaskEventCallback>& callback : TaskEventCallbacks->Callbacks)
			callback.Callback(eventList, callback.UserData);
	}

	if (!entitiesToUnsubscribe.empty())
		UnsubscribeEntities(entitiesToUnsubscribe);
}

// TODO: This should return whether it was actually successful (i.e. the entity exists)
void ConsoleMovementComponentManager::PathEntitiesTo(const gv::EntityList& entities,
                                                     const gv::PositionList& positions)
{
	if (entities.empty() || entities.size() != positions.size())
		return;

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		const gv::Entity& entityToMove = entities[i];
		const gv::Position& targetPosition = positions[i];

		// For now, entities which are not subscribed will be tossed out
		if (std::find(Subscribers.begin(), Subscribers.end(), entityToMove) == Subscribers.end())
			continue;

		for (MovementComponent& component : MovementComponents)
		{
			if (component.entity == entityToMove)
			{
				component.GoalWorldPosition = targetPosition;
			}
		}
	}
}
