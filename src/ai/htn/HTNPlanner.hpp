#pragma once

#include "HTNTypes.hpp"
#include "HTNTasks.hpp"

namespace Htn
{
struct GoalDecomposition
{
	// The GoalTask this decomposition came from
	GoalTask* DecomposedGoalTask;

	// The index to the Method used for the current decomposition
	int MethodIndex;

	// The tasks themselves created by the Method
	TaskCallList CallList;

	// The state and parameters at the time this decomposition took place
	WorldState InitialState;
	ParameterList Parameters;

	// State while the method is being decomposed
	WorldState WorkingState;

	// The result of this stack frame decomposition (only primitive tasks remaining)
	TaskCallList FinalCallList;
};

typedef std::vector<GoalDecomposition> GoalDecompositionStack;

class Planner
{
public:
	WorldState State;

	TaskCallList InitialCallList;

	// Filled out by PlanStep(); Should only be used once PlanStep returns PlanComplete
	TaskCallList FinalCallList;

	//
	// Settings to tweak how long you want a single PlanStep() to be
	//
	// Break immediately after a new method has been chosen for a goal
	bool BreakOnStackPush = true;
	// Break immediately after a method has failed to be decomposed
	bool BreakOnStackPop = true;
	// Break immediately after a compound task has been decomposed
	bool BreakOnCompoundDecomposition = false;
	// Break immediately after a primitive task has been applied
	bool BreakOnPrimitiveApply = false;

	bool DebugPrint = false;

	enum class Status
	{
		// Bad
		Failed_BadData = 0,
		Failed_NoPossiblePlan,
		Failed_NoTasks,

		Running_SuccessfulDecomposition,
		Running_SuccessfulPrimitive,
		Running_FailedMethodDecomposition,
		Running_FailedGoalDecomposition,

		// Done
		PlanComplete
	};

	Status PlanStep(void);

private:
	GoalDecompositionStack DecompositionStack;

	// Used for when all goals have been decomposed to manage mutable state
	WorldState StacklessState;

	// Copy of InitialCallList that Planner can fuck with
	TaskCallList WorkingCallList;
};
};