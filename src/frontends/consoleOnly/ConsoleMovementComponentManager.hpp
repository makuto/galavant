#pragma once

#include "entityComponentSystem/EntityTypes.hpp"
#include "entityComponentSystem/ComponentManager.hpp"
#include "world/Position.hpp"
#include "world/WorldResourceLocator.hpp"
#include "ai/htn/HTNTypes.hpp"
#include "game/agent/MovementManager.hpp"
#include "util/CallbackContainer.hpp"

struct MovementComponent
{
	gv::Entity entity;
	gv::Position Position;

	gv::Position GoalWorldPosition;
	float GoalManDistanceTolerance = 400.f;
	float MaxSpeed = 500.f;

	// If provided, this entity will be registered in the WorldResourceLocator under this type
	gv::WorldResourceType ResourceType = gv::WorldResourceType::None;

	// The last position we told the ResourceLocator we were at (used so that when we move we can
	// find the agent to move in ResourceLocator)
	gv::Position ResourcePosition;
};

typedef std::vector<MovementComponent> MovementComponentList;
typedef std::vector<MovementComponent*> MovementComponentRefList;

class ConsoleMovementComponentManager : public gv::ComponentManager, public gv::MovementManager
{
private:
	gv::CallbackContainer<Htn::TaskEventCallback>* TaskEventCallbacks;

	MovementComponentList MovementComponents;

protected:
	virtual void UnsubscribeEntitiesInternal(const gv::EntityList& entities);

public:
	ConsoleMovementComponentManager();
	virtual ~ConsoleMovementComponentManager() = default;

	void Initialize(gv::CallbackContainer<Htn::TaskEventCallback>* taskEventCallbacks);

	void CreateMovementComponents(const gv::EntityList& entities,
	                              MovementComponentRefList& newMovementComponents);

	virtual void Update(float deltaSeconds);

	// TODO: This should return whether it was actually successful (i.e. the entity exists)
	virtual void PathEntitiesTo(const gv::EntityList& entities, const gv::PositionList& positions);
	virtual void SetEntitySpeeds(const gv::EntityList& entities, const std::vector<float>& speeds);
};

extern ConsoleMovementComponentManager g_ConsoleMovementComponentManager;