#pragma once

#include "../../../ai/htn/HTNTypes.hpp"
#include "../../../ai/htn/HTNTasks.hpp"
#include "../../../ai/WorldState.hpp"
#include "../MovementManager.hpp"

namespace gv
{
// Parameters:
//  [0]: Resource type (int)
class FindResourceTask : public Htn::PrimitiveTask
{
public:
	FindResourceTask() = default;
	virtual ~FindResourceTask() = default;

	void Initialize();
	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const Htn::ParameterList& parameters) const;
	virtual void ApplyStateChange(gv::WorldState& state, const Htn::ParameterList& parameters);
	virtual Htn::TaskExecuteStatus Execute(gv::WorldState& state,
	                                       const Htn::ParameterList& parameters);
};

// Parameters:
//  None - Entity to move and target come from world state
class MoveToTask : public Htn::PrimitiveTask
{
private:
	MovementManager* movementManager;

public:
	MoveToTask() = default;
	virtual ~MoveToTask() = default;

	void Initialize(MovementManager* newMovementManager);
	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const Htn::ParameterList& parameters) const;
	virtual void ApplyStateChange(gv::WorldState& state, const Htn::ParameterList& parameters);
	virtual Htn::TaskExecuteStatus Execute(gv::WorldState& state,
	                                       const Htn::ParameterList& parameters);
};

// Parameters:
//  [0]: Resource type (int)
class GetResourceTask : public Htn::CompoundTask
{
private:
	// Should this be how it is? Should tasks be singletons?
	Htn::Task FindResourceTaskRef;
	Htn::Task MoveToTaskRef;

public:
	GetResourceTask() = default;
	virtual ~GetResourceTask() = default;

	void Initialize(FindResourceTask* newFindResourceTask, MoveToTask* newMoveToTask);
	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const Htn::ParameterList& parameters) const;
	virtual bool Decompose(Htn::TaskCallList& taskCallList, const gv::WorldState& state,
	                       const Htn::ParameterList& parameters);
};
}