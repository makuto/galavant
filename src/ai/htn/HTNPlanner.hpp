#pragma once

#include "HTNTypes.hpp"
#include "HTNTasks.hpp"
#include "../WorldState.hpp"

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
	gv::WorldState InitialState;
	ParameterList Parameters;

	// State while the method is being decomposed
	gv::WorldState WorkingState;

	// The result of this stack frame decomposition (only primitive tasks remaining)
	TaskCallList FinalCallList;
};

typedef std::vector<GoalDecomposition> GoalDecompositionStack;

/* -- Planner --
Given the world state and a set of goal tasks, decompose them into a list of primitive tasks which,
when executed, will result in the desired goal tasks being completed. Note that the initial goal
task set can include compound and primitive tasks.

TODO: Either make planner allow making multiple plans with the same instance, or make it clear
that it is for a single plan only
*/
class Planner
{
public:
	Planner() = default;
	~Planner() = default;

	gv::WorldState State;

	TaskCallList InitialCallList;

	// Filled out by PlanStep(); Should only be used once PlanStep returns PlanComplete
	TaskCallList FinalCallList;

	//
	// Settings to tweak how long you want a single PlanStep() to be
	//
	// Break whenever a stack action has occurred (a goal has been decomposed, a method has finished
	// decomposition, or a method failed to decompose)
	bool BreakOnStackAction = true;
	// Break immediately after a compound task has been decomposed
	bool BreakOnCompoundDecomposition = false;
	// Break immediately after a primitive task has been applied
	bool BreakOnPrimitiveApply = false;
	// Break whenever a goal or method failed to decompose
	bool BreakOnFailedDecomposition = false;

	// Print various details about the status of the stack etc.
	bool DebugPrint = false;

	enum class Status
	{
		// Bad
		Failed_BadData = 0,
		Failed_NoPossiblePlan,
		Failed_NoTasks,

		Running_EnumBegin,

		Running_SuccessfulDecomposition,
		Running_SuccessfulDecompositionStackPush,
		Running_SuccessfulDecompositionStackPop,
		Running_SuccessfulPrimitive,
		Running_FailedMethodDecomposition,
		Running_FailedGoalDecomposition,

		Running_EnumEnd,

		// Done
		PlanComplete
	};

	bool IsPlannerRunning();
	bool IsPlannerRunning(Status status);

	Status CurrentStatus;

	Status PlanStep();

private:
	GoalDecompositionStack DecompositionStack;

	// Used for when all goals have been decomposed to manage mutable state
	gv::WorldState StacklessState;

	// Copy of InitialCallList that Planner can fuck with
	TaskCallList WorkingCallList;

	Status PlanStep_StackFrame();
	Status PlanStep_BottomLevel();
};
};