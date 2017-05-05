#pragma once

#include "../../../ai/htn/HTNTypes.hpp"
#include "../../../ai/htn/HTNTasks.hpp"
#include "../../../ai/WorldState.hpp"

#include "../../InteractComponentManager.hpp"

namespace gv
{
// Parameters:
//  None
// Dependent WorldState:
//	TargetEntity
class InteractPickupTask : public Htn::PrimitiveTask
{
private:
	InteractComponentManager* InteractManager;
public:
	InteractPickupTask() = default;
	virtual ~InteractPickupTask() = default;

	void Initialize(InteractComponentManager* newInteractManager);
	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const Htn::ParameterList& parameters) const;
	virtual void ApplyStateChange(gv::WorldState& state, const Htn::ParameterList& parameters);
	virtual Htn::TaskExecuteStatus Execute(gv::WorldState& state,
	                                       const Htn::ParameterList& parameters);
};
}
