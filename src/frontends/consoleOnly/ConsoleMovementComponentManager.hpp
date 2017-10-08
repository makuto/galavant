#pragma once

#include "entityComponentSystem/EntityTypes.hpp"
#include "entityComponentSystem/ComponentManager.hpp"
#include "world/Position.hpp"
#include "world/WorldResourceLocator.hpp"
#include "ai/htn/HTNTypes.hpp"
#include "game/agent/MovementManager.hpp"
#include "util/CallbackContainer.hpp"

class ConsoleMovementComponentManager : public gv::ComponentManager, public gv::MovementManager
{
private:
	gv::CallbackContainer<Htn::TaskEventCallback>* TaskEventCallbacks;

public:
	ConsoleMovementComponentManager();
	virtual ~ConsoleMovementComponentManager() = default;

	void Initialize(gv::CallbackContainer<Htn::TaskEventCallback>* taskEventCallbacks);
	virtual void Update(float deltaSeconds);

	// TODO: This should return whether it was actually successful (i.e. the entity exists)
	virtual void PathEntitiesTo(const gv::EntityList& entities, const gv::PositionList& positions);
};

extern ConsoleMovementComponentManager g_ConsoleMovementComponentManager;