#include "HTNPlanner.hpp"

#include "HTNTypes.hpp"
#include "HTNTasks.hpp"

#include <iostream>

namespace Htn
{
bool DecomposeGoalTask(GoalDecompositionStack& decompositionStack, GoalTask* goalTask,
                       int methodIndex, ParameterList& parameters, WorldState& state)
{
	GoalDecomposition decomposition;
	// GoalTask* goalTask = currentTask->GetGoal();
	decomposition.DecomposedGoalTask = goalTask;
	decomposition.MethodIndex = methodIndex;
	// How the hell are parameters off of goals used? Fuck
	// Well, a method could be restricted to a single Primitive Task or a
	// CompoundTask
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
                           TaskArguments& taskArguments)
{
	if (!compoundTask->StateMeetsPreconditions(taskArguments))
		return false;

	return compoundTask->Decompose(compoundDecompositions, taskArguments);
}

Planner::Status Planner::PlanStep(void)
{
	// When the stack is empty, find a goal task to push onto the task or add tasks as per usual
	// If any Decompositions or Preconditions fail, we must fail the entire plan because we have
	//  no alternate methods
	if (DecompositionStack.empty())
	{
		if (InitialCallList.empty())
			return Status::Failed_NoTasks;
		else if (WorkingCallList.empty() && FinalCallList.empty())
		{
			WorkingCallList = InitialCallList;
			StacklessState = State;
		}
		else if (WorkingCallList.empty() && !FinalCallList.empty())
			return Status::PlanComplete;

		TaskCallList compoundDecompositions;

		if (DebugPrint)
		{
			std::cout << "\nPlanStep()\nWorkingCallList.size() = " << WorkingCallList.size()
			          << "\n";
			printTaskCallList(WorkingCallList);
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
			// NullEntity = TODO
			TaskArguments taskArguments = {0, StacklessState, currentTaskCall.Parameters};

			if (DebugPrint)
				std::cout << "TaskType currentTaskType = " << (int)currentTaskType << "\n";

			switch (currentTaskType)
			{
				case TaskType::Goal:
				{
					if (DebugPrint)
						std::cout << "Goal\n";
					GoalTask* goalTask = currentTask->GetGoal();

					if (!DecomposeGoalTask(DecompositionStack, goalTask, 0,
					                       currentTaskCall.Parameters, StacklessState))
						return Status::Failed_NoPossiblePlan;

					if (BreakOnStackPush)
					{
						WorkingCallList.erase(currentTaskCallIter);
						return Status::Running_SuccessfulDecomposition;
					}
					// TODO: IMPORTANT! This code will collapse horribly if I don't now start
					// processing the stack
					WorkingCallList.erase(currentTaskCallIter);
					return Status::Running_SuccessfulDecomposition;
				}
				break;
				case TaskType::Primitive:
				{
					if (DebugPrint)
						std::cout << "Primitive\n";
					PrimitiveTask* primitiveTask = currentTask->GetPrimitive();
					if (!primitiveTask->StateMeetsPreconditions(taskArguments))
						return Status::Failed_NoPossiblePlan;

					// If at top level, apply primitive tasks immediately to the stackless state
					primitiveTask->ApplyStateChange(taskArguments);
					StacklessState = taskArguments.worldState;

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
					if (DebugPrint)
						std::cout << "Compound\n";
					CompoundTask* compoundTask = currentTask->GetCompound();
					if (!DecomposeCompoundTask(compoundDecompositions, compoundTask, taskArguments))
						return Status::Failed_NoPossiblePlan;

					currentTaskCallIter = WorkingCallList.erase(currentTaskCallIter);

					// we need to push our decomposition to our call list, but we're iterating on
					//  it. By pushing our decomposition to compoundDecompositions, we break out of
					//  the loop and tack it on there
				}
				break;
				default:
					return Status::Failed_BadData;
			}

			if (compoundDecompositions.size())
			{
				if (DebugPrint)
				{
					std::cout << "compoundDecompositions.size() = " << compoundDecompositions.size()
					          << "\n";
				}
				WorkingCallList.insert(WorkingCallList.begin(), compoundDecompositions.begin(),
				                       compoundDecompositions.end());
				// We have to break here because we are adding things to the list we're iterating
				//  on; We'll process the tasks next Step
				// break; // UPDATE: Eventually we'll need a way to start over the loop if the
				// caller wants whole plan in one call
				if (DebugPrint)
					std::cout << "PlanStep Done\n";

				return Status::Running_SuccessfulDecomposition;
			}

			if (DebugPrint)
				std::cout << "Loop Done\n";
		}
	}
	else
	{
		// Remember: If goal fails to decompose and goal is bottom of stack, fail
		GoalDecompositionStack::iterator currentStackFrameIter = DecompositionStack.end() - 1;
		GoalDecomposition& currentStackFrame = *currentStackFrameIter;

		if (DebugPrint)
		{
			std::cout << "\nPlanStep()\ncurrentStackFrame.CallList.size() = "
			          << currentStackFrame.CallList.size() << "\n";
			printTaskCallList(currentStackFrame.CallList);

			std::cout << "Stack Depth: ";
			for (unsigned int i = 0; i < DecompositionStack.size(); i++)
				std::cout << "=";
			std::cout << "\n";

			{
				std::cout << "----Fullstack working lists\n";
				std::cout << "[0]\n";
				printTaskCallList(WorkingCallList);
				int i = 1;
				for (GoalDecomposition& stackFrame : DecompositionStack)
				{
					std::cout << "[" << i++ << "]\n";
					printTaskCallList(stackFrame.CallList);
				}
				std::cout << "----\n";
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
			// NullEntity = TODO
			TaskArguments taskArguments = {0, currentStackFrame.WorkingState,
			                               currentTaskCall.Parameters};

			if (DebugPrint)
				std::cout << "TaskType currentTaskType = " << (int)currentTaskType << "\n";

			switch (currentTaskType)
			{
				case TaskType::Goal:
				{
					if (DebugPrint)
						std::cout << "Goal\n";
					GoalTask* goalTask = currentTask->GetGoal();

					// TODO erase ahead of time because fuck
					// Strange things are afoot when we push to stack
					currentStackFrame.CallList.erase(currentTaskCallIter);

					// Perform the decomposition. This function only fails if the method at method
					// index doesn't exist. This means that the goal task has no alternative options
					if (!DecomposeGoalTask(DecompositionStack, goalTask, 0,
					                       currentTaskCall.Parameters,
					                       currentStackFrame.WorkingState))
					{
						methodFailed = true;
						break;
					}

					// This code is wrong. I'm not sure why.
					// Pushing to the stack invalidates our currentStackFrame reference; update it
					// What the actual fuck (updating the ref doesn't fix the fucking thing)
					/*std::cout << "Ref updating from " << &currentStackFrame << " to "
					          << &(*(DecompositionStack.end() - 1)) << "\n";
					currentStackFrameIter = DecompositionStack.end() - 1;
					currentStackFrame = *currentStackFrameIter;*/

					if (BreakOnStackPush)
					{
						// currentStackFrame.CallList.erase(currentTaskCallIter);
						return Status::Running_SuccessfulDecomposition;
					}
					// TODO: IMPORTANT! This code will collapse horribly if I don't now start
					// processing the stack
					// currentStackFrame.CallList.erase(currentTaskCallIter);
					return Status::Running_SuccessfulDecomposition;
				}
				break;
				case TaskType::Primitive:
				{
					if (DebugPrint)
						std::cout << "Primitive\n";
					PrimitiveTask* primitiveTask = currentTask->GetPrimitive();
					if (!primitiveTask->StateMeetsPreconditions(taskArguments))
					{
						methodFailed = true;
						break;
					}

					primitiveTask->ApplyStateChange(taskArguments);
					currentStackFrame.WorkingState = taskArguments.worldState;

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
					if (DebugPrint)
						std::cout << "Compound\n";
					CompoundTask* compoundTask = currentTask->GetCompound();

					if (!DecomposeCompoundTask(compoundDecompositions, compoundTask, taskArguments))
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
				if (DebugPrint)
					std::cout << "Method failed decomposition\n";
				// Clear stack frame
				currentStackFrame.CallList.clear();
				currentStackFrame.FinalCallList.clear();
				// currentStackFrame.Parameters.clear(); // Little weird (implicit assumption that
				// GoalTask uses exact parameters)
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

				// TODO: make this optional return
				return Status::Running_FailedMethodDecomposition;
			}

			if (compoundDecompositions.size())
			{
				if (DebugPrint)
				{
					std::cout << "compoundDecompositions.size() = " << compoundDecompositions.size()
					          << "\n";
					std::cout << "currentStackFrame.CallList.size() = "
					          << currentStackFrame.CallList.size() << "\n";
					std::cout << "Decomposition:\n";
					printTaskCallList(compoundDecompositions);
				}
				currentStackFrame.CallList.insert(currentStackFrame.CallList.begin(),
				                                  compoundDecompositions.begin(),
				                                  compoundDecompositions.end());
				// We have to break here because we are adding things to the list we're iterating
				//  on; We'll process the tasks next Step
				// break; // UPDATE: TODO: Eventually we'll need a way to start over the loop if the
				// caller wants whole plan in one call
				if (DebugPrint)
					std::cout << "PlanStep Done\n";
				return Status::Running_SuccessfulDecomposition;
			}

			if (DebugPrint)
				std::cout << "Loop Done\n";
		}

		// Finished processing this stack frame
		if (currentStackFrame.CallList.empty() && !currentStackFrame.FinalCallList.empty())
		{
			bool onlyStackFrame = DecompositionStack.size() == 1;
			TaskCallList* parentFinalCallList = nullptr;
			WorldState* parentWorkingState = nullptr;

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
				std::cout << "Collapsing stack frame. Adding List:\n";
				printTaskCallList(currentStackFrame.FinalCallList);
				std::cout << "To parent:\n";
				printTaskCallList(*parentFinalCallList);
			}
			// Should this be appended?
			parentFinalCallList->insert(parentFinalCallList->end(),
			                            currentStackFrame.FinalCallList.begin(),
			                            currentStackFrame.FinalCallList.end());

			*parentWorkingState = currentStackFrame.WorkingState;

			DecompositionStack.pop_back();

			if (DebugPrint)
				std::cout << "Frame Done\n";

			if (BreakOnStackPop)
				return Status::Running_SuccessfulDecomposition;

			// TODO: Continue processing the stack
			return Status::Running_SuccessfulDecomposition;
		}
	}

	return Status::PlanComplete;
}
}