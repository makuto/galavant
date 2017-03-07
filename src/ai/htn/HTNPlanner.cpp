#include "HTNPlanner.hpp"

#include "../../util/Logging.hpp"

#include "HTNTypes.hpp"
#include "HTNTasks.hpp"

namespace Htn
{
bool DecomposeGoalTask(GoalDecompositionStack& decompositionStack, GoalTask* goalTask,
                       int methodIndex, ParameterList& parameters, gv::WorldState& state)
{
	GoalDecomposition decomposition;
	decomposition.DecomposedGoalTask = goalTask;
	decomposition.MethodIndex = methodIndex;
	decomposition.Parameters = parameters;
	decomposition.InitialState = state;

	// Perform the decomposition
	if (goalTask->DecomposeMethodAtIndex(decomposition.CallList, decomposition.MethodIndex,
	                                     decomposition.Parameters))
	{
		decompositionStack.push_back(decomposition);
		return true;
	}

	return false;
}

bool DecomposeCompoundTask(TaskCallList& compoundDecompositions, CompoundTask* compoundTask,
                           const gv::WorldState& state, const ParameterList& parameters)
{
	if (!compoundTask->StateMeetsPreconditions(state, parameters))
		return false;

	return compoundTask->Decompose(compoundDecompositions, state, parameters);
}

bool Planner::IsPlannerRunning()
{
	return IsPlannerRunning(CurrentStatus);
}

bool Planner::IsPlannerRunning(Status status)
{
	return (status >= Status::Running_EnumBegin && status <= Status::Running_EnumEnd);
}

// When the stack is empty, find a goal task to push onto the task or add tasks as per usual
// If any Decompositions or Preconditions fail, we must fail the entire plan because we have
//  no alternate methods
Planner::Status Planner::PlanStep_BottomLevel()
{
	if (InitialCallList.empty())
		return Status::Failed_NoTasks;
	else if (WorkingCallList.empty() && FinalCallList.empty())
	{
		// First time the planner has been stepped
		WorkingCallList = InitialCallList;
		StacklessState = State;
	}
	else if (WorkingCallList.empty() && !FinalCallList.empty())
		return Status::PlanComplete;

	TaskCallList compoundDecompositions;

	if (DebugPrint)
	{
		LOGD << "PlanStep() WorkingCallList.size() = " << WorkingCallList.size();
		PrintTaskCallList(WorkingCallList);
	}

	for (TaskCallListIterator currentTaskCallIter = WorkingCallList.begin();
	     currentTaskCallIter != WorkingCallList.end();
	     currentTaskCallIter = WorkingCallList.erase(currentTaskCallIter))
	{
		TaskCall currentTaskCall = (*currentTaskCallIter);
		Task* currentTask = currentTaskCall.TaskToCall;
		if (!currentTask)
			continue;
		TaskType currentTaskType = currentTask->GetType();

		LOGD_IF(DebugPrint) << "TaskType currentTaskType = " << (int)currentTaskType;

		switch (currentTaskType)
		{
			case TaskType::Goal:
			{
				LOGD_IF(DebugPrint) << "Goal";
				GoalTask* goalTask = currentTask->GetGoal();

				if (!DecomposeGoalTask(DecompositionStack, goalTask, 0, currentTaskCall.Parameters,
				                       StacklessState))
					return Status::Failed_NoPossiblePlan;

				WorkingCallList.erase(currentTaskCallIter);
				return Status::Running_SuccessfulDecompositionStackPush;
			}
			break;
			case TaskType::Primitive:
			{
				LOGD_IF(DebugPrint) << "Primitive";
				PrimitiveTask* primitiveTask = currentTask->GetPrimitive();
				if (!primitiveTask->StateMeetsPreconditions(StacklessState,
				                                            currentTaskCall.Parameters))
					return Status::Failed_NoPossiblePlan;

				// If at top level, apply primitive tasks immediately to the stackless state
				primitiveTask->ApplyStateChange(StacklessState, currentTaskCall.Parameters);

				FinalCallList.push_back(currentTaskCall);

				if (BreakOnPrimitiveApply)
				{
					WorkingCallList.erase(currentTaskCallIter);
					return Status::Running_SuccessfulPrimitive;
				}
				// Keep processing tasks otherwise
			}
			break;
			case TaskType::Compound:
			{
				LOGD_IF(DebugPrint) << "Compound";
				CompoundTask* compoundTask = currentTask->GetCompound();

				// we need to push our decomposition to our call list, but we're iterating on
				//  it. By pushing our decomposition to compoundDecompositions in
				//  DecomposeCompoundTask(), we'll then break out of the loop and tack it on there
				if (!DecomposeCompoundTask(compoundDecompositions, compoundTask, StacklessState,
				                           currentTaskCall.Parameters))
					return Status::Failed_NoPossiblePlan;

				currentTaskCallIter = WorkingCallList.erase(currentTaskCallIter);
			}
			break;
			default:
				return Status::Failed_BadData;
		}

		if (compoundDecompositions.size())
		{
			LOGD_IF(DebugPrint) << "compoundDecompositions.size() = "
			                    << compoundDecompositions.size();

			WorkingCallList.insert(WorkingCallList.begin(), compoundDecompositions.begin(),
			                       compoundDecompositions.end());

			LOGD_IF(DebugPrint) << "PlanStep Done";

			// We have to break here because we are adding things to the list we're iterating
			//  on; We'll process the tasks next Step
			return Status::Running_SuccessfulDecomposition;
		}

		LOGD_IF(DebugPrint) << "Loop Done";
	}

	return Status::PlanComplete;
}

Planner::Status Planner::PlanStep_StackFrame()
{
	// Remember: If goal fails to decompose and goal is bottom of stack, fail
	GoalDecompositionStack::iterator currentStackFrameIter = DecompositionStack.end() - 1;
	GoalDecomposition& currentStackFrame = *currentStackFrameIter;

	if (DebugPrint)
	{
		LOGD << "PlanStep() currentStackFrame.CallList.size() = "
		     << currentStackFrame.CallList.size();
		PrintTaskCallList(currentStackFrame.CallList);

		LOGD << "Stack Depth: ";
		for (unsigned int i = 0; i < DecompositionStack.size(); i++)
			LOGD << "=";

		{
			LOGD << "----Fullstack working lists";
			LOGD << "[0]";
			PrintTaskCallList(WorkingCallList);
			int i = 1;
			for (GoalDecomposition& stackFrame : DecompositionStack)
			{
				LOGD << "[" << i++ << "]";
				PrintTaskCallList(stackFrame.CallList);
			}
			LOGD << "----";
		}
	}

	TaskCallList compoundDecompositions;
	bool methodFailed = false;

	for (TaskCallListIterator currentTaskCallIter = currentStackFrame.CallList.begin();
	     currentTaskCallIter != currentStackFrame.CallList.end();
	     currentTaskCallIter = currentStackFrame.CallList.erase(currentTaskCallIter))
	{
		TaskCall currentTaskCall = (*currentTaskCallIter);
		Task* currentTask = currentTaskCall.TaskToCall;
		if (!currentTask)
			continue;
		TaskType currentTaskType = currentTask->GetType();

		LOGD_IF(DebugPrint) << "TaskType currentTaskType = " << (int)currentTaskType;

		switch (currentTaskType)
		{
			case TaskType::Goal:
			{
				LOGD_IF(DebugPrint) << "Goal";
				GoalTask* goalTask = currentTask->GetGoal();

				// TODO erase ahead of time because fuck
				// Strange things are afoot when we push to stack
				currentStackFrame.CallList.erase(currentTaskCallIter);

				// Perform the decomposition. This function only fails if the method at method
				// index doesn't exist. This means that the goal task has no alternative options
				if (!DecomposeGoalTask(DecompositionStack, goalTask, 0, currentTaskCall.Parameters,
				                       currentStackFrame.WorkingState))
				{
					methodFailed = true;
					break;
				}

				// This code is wrong. I'm not sure why.
				// Pushing to the stack invalidates our currentStackFrame reference; update it
				// What the actual fuck (updating the ref doesn't fix the fucking thing)
				/*LOGD << "Ref updating from " << &currentStackFrame << " to "
				          << &(*(DecompositionStack.end() - 1));
				currentStackFrameIter = DecompositionStack.end() - 1;
				currentStackFrame = *currentStackFrameIter;*/

				return Status::Running_SuccessfulDecompositionStackPush;
			}
			break;
			case TaskType::Primitive:
			{
				LOGD_IF(DebugPrint) << "Primitive";
				PrimitiveTask* primitiveTask = currentTask->GetPrimitive();
				if (!primitiveTask->StateMeetsPreconditions(currentStackFrame.WorkingState,
				                                            currentTaskCall.Parameters))
				{
					methodFailed = true;
					break;
				}

				primitiveTask->ApplyStateChange(currentStackFrame.WorkingState,
				                                currentTaskCall.Parameters);

				currentStackFrame.FinalCallList.push_back(currentTaskCall);

				if (BreakOnPrimitiveApply)
				{
					currentStackFrame.CallList.erase(currentTaskCallIter);
					return Status::Running_SuccessfulPrimitive;
				}
				// Keep processing tasks otherwise
			}
			break;
			case TaskType::Compound:
			{
				LOGD_IF(DebugPrint) << "Compound";
				CompoundTask* compoundTask = currentTask->GetCompound();

				if (!DecomposeCompoundTask(compoundDecompositions, compoundTask,
				                           currentStackFrame.WorkingState,
				                           currentTaskCall.Parameters))
				{
					methodFailed = true;
					break;
				}

				currentTaskCallIter = currentStackFrame.CallList.erase(currentTaskCallIter);

				// we need to push our decomposition to our call list, but we're iterating on
				//  it. By pushing our decomposition to compoundDecompositions, we break out of
				//  the loop and tack it on there
			}
			break;
			default:
				return Status::Failed_BadData;
		}

		if (methodFailed)
		{
			LOGD_IF(DebugPrint) << "Method failed decomposition";
			// Clear stack frame
			currentStackFrame.CallList.clear();
			currentStackFrame.FinalCallList.clear();
			currentStackFrame.WorkingState = currentStackFrame.InitialState;

			// Try the next method
			currentStackFrame.MethodIndex++;
			if (!currentStackFrame.DecomposedGoalTask->DecomposeMethodAtIndex(
			        currentStackFrame.CallList, currentStackFrame.MethodIndex,
			        currentStackFrame.Parameters))
			{
				DecompositionStack.pop_back();
				return Status::Running_FailedGoalDecomposition;
			}

			// We have failed to decompose the previous method, but successfully got the next
			// method. Return to let the Planner know what just happened
			return Status::Running_FailedMethodDecomposition;
		}

		if (compoundDecompositions.size())
		{
			if (DebugPrint)
			{
				LOGD << "compoundDecompositions.size() = " << compoundDecompositions.size();
				LOGD << "currentStackFrame.CallList.size() = " << currentStackFrame.CallList.size();
				LOGD << "Decomposition:";
				PrintTaskCallList(compoundDecompositions);
			}
			currentStackFrame.CallList.insert(currentStackFrame.CallList.begin(),
			                                  compoundDecompositions.begin(),
			                                  compoundDecompositions.end());
			LOGD_IF(DebugPrint) << "PlanStep Done";

			// We have to break here because we are adding things to the list we're iterating
			//  on; We'll process the tasks next Step
			return Status::Running_SuccessfulDecomposition;
		}

		LOGD_IF(DebugPrint) << "Loop Done";
	}

	// Finished processing this stack frame
	if (currentStackFrame.CallList.empty() && !currentStackFrame.FinalCallList.empty())
	{
		bool onlyStackFrame = DecompositionStack.size() == 1;
		TaskCallList* parentFinalCallList = nullptr;
		gv::WorldState* parentWorkingState = nullptr;

		// If this is the only frame on the stack, its call list and working state goes to the
		// plan, else, the previous stack
		if (onlyStackFrame)
		{
			parentFinalCallList = &FinalCallList;
			parentWorkingState = &StacklessState;
		}
		else
		{
			GoalDecomposition& previousStackFrame = *(currentStackFrameIter - 1);

			parentFinalCallList = &previousStackFrame.FinalCallList;
			parentWorkingState = &previousStackFrame.WorkingState;
		}

		if (DebugPrint)
		{
			LOGD << "Collapsing stack frame. Adding List:";
			PrintTaskCallList(currentStackFrame.FinalCallList);
			LOGD << "To parent:";
			PrintTaskCallList(*parentFinalCallList);
		}

		parentFinalCallList->insert(parentFinalCallList->end(),
		                            currentStackFrame.FinalCallList.begin(),
		                            currentStackFrame.FinalCallList.end());

		*parentWorkingState = currentStackFrame.WorkingState;

		DecompositionStack.pop_back();

		LOGD_IF(DebugPrint) << "Frame Done";

		return Status::Running_SuccessfulDecompositionStackPop;
	}

	return Status::PlanComplete;
}

// TODO: Pool various task lists?
// TODO: Pull more things out into functions, if possible. It's bad that whenever I make a change to
// something I have to change it in two places
Planner::Status Planner::PlanStep()
{
	Status status = Status::Failed_NoPossiblePlan;

	// Continue stepping the plan until a non-running state is complete or a status is returned
	// which matches the user-specified break conditions
	do
	{
		if (DecompositionStack.empty())
			status = PlanStep_BottomLevel();
		else
			status = PlanStep_StackFrame();

		// The user can decided how much planning happens in a single step; conditionally stop
		// stepping here based on their settings
		if ((BreakOnCompoundDecomposition && status == Status::Running_SuccessfulDecomposition) ||
		    (BreakOnPrimitiveApply && status == Status::Running_SuccessfulPrimitive) ||
		    (BreakOnStackAction && (status == Status::Running_SuccessfulDecompositionStackPush ||
		                            status == Status::Running_SuccessfulDecompositionStackPop ||
		                            status == Status::Running_FailedGoalDecomposition)) ||
		    (BreakOnFailedDecomposition && (status == Status::Running_FailedGoalDecomposition ||
		                                    status == Status::Running_FailedMethodDecomposition)))
			break;

	} while (IsPlannerRunning(status));

	CurrentStatus = status;
	return status;
}
}