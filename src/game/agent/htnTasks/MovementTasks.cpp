#include "MovementTasks.hpp"

#include "util/Logging.hpp"
#include "world/Position.hpp"
#include "world/WorldResourceLocator.hpp"

namespace gv
{
bool FindResourceTask::StateMeetsPreconditions(const gv::WorldState& state,
                                               const Htn::ParameterList& parameters) const
{
	return parameters.size() == 1 && parameters[0].Type == Htn::Parameter::ParamType::Int &&
	       gv::WorldResourceLocator::ResourceExistsInWorld(
	           (gv::WorldResourceType)parameters[0].IntValue);
}

void FindResourceTask::ApplyStateChange(gv::WorldState& state, const Htn::ParameterList& parameters)
{
	// TODO: Should this be the case? Should StateMeetsPreconditions find the position? This isn't
	// that much of a problem if ResourceLocator caches searches
	float manhattanTo = 0.f;
	gv::WorldResourceLocator::Resource* resource = gv::WorldResourceLocator::FindNearestResource(
	    (gv::WorldResourceType)parameters[0].IntValue, state.SourceAgent.position, false,
	    manhattanTo);
	if (resource)
	{
		state.SourceAgent.TargetPosition = resource->position;
		state.SourceAgent.TargetEntity = resource->entity;
	}
}

Htn::TaskExecuteStatus FindResourceTask::Execute(gv::WorldState& state,
                                                 const Htn::ParameterList& parameters)
{
	float manhattanTo = 0.f;
	gv::WorldResourceLocator::Resource* resource = gv::WorldResourceLocator::FindNearestResource(
	    (gv::WorldResourceType)parameters[0].IntValue, state.SourceAgent.position, false,
	    manhattanTo);
	if (resource)
	{
		LOGD << "Found resource at " << state.SourceAgent.TargetPosition;
		// TODO: Execute and ApplyStateChange duplicate code
		state.SourceAgent.TargetPosition = resource->position;
		state.SourceAgent.TargetEntity = resource->entity;
		Htn::TaskExecuteStatus status{Htn::TaskExecuteStatus::ExecutionStatus::Succeeded};
		return status;
	}
	else
		LOGD << "Couldn't find resource!";

	LOGD << "Failed to find resource";
	Htn::TaskExecuteStatus status{Htn::TaskExecuteStatus::ExecutionStatus::Failed};
	return status;
}

void MoveToTask::Initialize(MovementManager* newMovementManager)
{
	movementManager = newMovementManager;
}

bool MoveToTask::StateMeetsPreconditions(const gv::WorldState& state,
                                         const Htn::ParameterList& parameters) const
{
	// We're good to move to a position as long as we have a target
	if (state.SourceAgent.TargetPosition)
		return true;

	return false;
}

void MoveToTask::ApplyStateChange(gv::WorldState& state, const Htn::ParameterList& parameters)
{
	state.SourceAgent.position = state.SourceAgent.TargetPosition;
}

Htn::TaskExecuteStatus MoveToTask::Execute(gv::WorldState& state,
                                           const Htn::ParameterList& parameters)
{
	if (movementManager)
	{
		gv::EntityList entitiesToMove{state.SourceAgent.SourceEntity};
		std::vector<gv::Position> positions{state.SourceAgent.TargetPosition};
		LOGD << "Moving Ent[" << state.SourceAgent.SourceEntity << "] to "
		     << state.SourceAgent.TargetPosition;
		movementManager->PathEntitiesTo(entitiesToMove, positions);
		Htn::TaskExecuteStatus status{Htn::TaskExecuteStatus::ExecutionStatus::WaitForEvent};
		return status;
	}

	Htn::TaskExecuteStatus status{Htn::TaskExecuteStatus::ExecutionStatus::Failed};
	return status;
}

void GetResourceTask::Initialize(FindResourceTask* newFindResourceTask, MoveToTask* newMoveToTask)
{
	FindResourceTaskRef.Initialize(newFindResourceTask);
	MoveToTaskRef.Initialize(newMoveToTask);
}

bool GetResourceTask::StateMeetsPreconditions(const gv::WorldState& state,
                                              const Htn::ParameterList& parameters) const
{
	// TODO: What is the purpose of the compound task checking preconditions if they'll be near
	// identical to the combined primitive task preconditions?
	bool parametersValid =
	    parameters.size() == 1 && parameters[0].Type == Htn::Parameter::ParamType::Int;
	LOGD_IF(!parametersValid) << "GetResourceTask parameters invalid! Expected Int";
	return parametersValid;
}

bool GetResourceTask::Decompose(Htn::TaskCallList& taskCallList, const gv::WorldState& state,
                                const Htn::ParameterList& parameters)
{
	Htn::ParameterList findResourceParams = parameters;
	Htn::ParameterList moveToParams;

	Htn::TaskCall FindResourceTaskCall{&FindResourceTaskRef, findResourceParams};
	Htn::TaskCall MoveToTaskCall{&MoveToTaskRef, moveToParams};

	// TODO: Make it clear that users should only ever push to this list
	taskCallList.push_back(FindResourceTaskCall);
	taskCallList.push_back(MoveToTaskCall);

	return true;
}
}