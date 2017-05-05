#include "InteractTasks.hpp"

#include "util/Logging.hpp"
#include "world/Position.hpp"
#include "world/WorldResourceLocator.hpp"

namespace gv
{
void InteractPickupTask::Initialize(InteractComponentManager* newInteractManager)
{
	InteractManager = newInteractManager;
}

bool InteractPickupTask::StateMeetsPreconditions(const gv::WorldState& state,
                                                 const Htn::ParameterList& parameters) const
{
	return state.SourceAgent.TargetEntity && InteractManager &&
	       InteractManager->IsSubscribed(state.SourceAgent.TargetEntity);
}

void InteractPickupTask::ApplyStateChange(gv::WorldState& state,
                                          const Htn::ParameterList& parameters)
{
	// No WorldState records needs yet
}

Htn::TaskExecuteStatus InteractPickupTask::Execute(gv::WorldState& state,
                                                   const Htn::ParameterList& parameters)
{
	// TODO: should the PlanComponentManager call this before executing any task?
	if (StateMeetsPreconditions(state, parameters))
	{
		if (InteractManager->PickupDirect(state.SourceAgent.TargetEntity,
		                                  state.SourceAgent.SourceEntity))
		{
			LOGD << "Found resource at " << state.SourceAgent.TargetPosition;
			Htn::TaskExecuteStatus status{Htn::TaskExecuteStatus::ExecutionStatus::Succeeded};
			return status;
		}
	}
	else
		LOGD << "State no longer matches preconditions";

	LOGD << "Failed to pickup";
	Htn::TaskExecuteStatus status{Htn::TaskExecuteStatus::ExecutionStatus::Failed};
	return status;
}
}