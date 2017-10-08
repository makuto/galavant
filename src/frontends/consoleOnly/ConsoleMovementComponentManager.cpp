#include "ConsoleMovementComponentManager.hpp"

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

void ConsoleMovementComponentManager::Update(float deltaSeconds)
{
}

// TODO: This should return whether it was actually successful (i.e. the entity exists)
void ConsoleMovementComponentManager::PathEntitiesTo(const gv::EntityList& entities,
                                                     const gv::PositionList& positions)
{
}
