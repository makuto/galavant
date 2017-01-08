#include "HTNPlanner.hpp"

#include "HTNTypes.hpp"
#include "HTNTasks.hpp"

#include <iostream>

namespace Htn
{
/*bool DecomposeGoalTask(TaskList& taskList, Task* task, const TaskArguments& taskArguments)
{
    GoalTask* goalTask = task->Goal;
    return false;  // TODO
}

bool DecomposeCompoundTask(TaskList& taskList, Task* task, const TaskArguments& taskArguments)
{
    CompoundTask* compoundTask = task->Compound;

    if (compoundTask->StateMeetsPreconditions(taskArguments))
        // Decomposition is determined by the task itself
        return compoundTask->Decompose(taskList, taskArguments);

    return false;
}*/

PlanStepStatus PlanStep(PlanState& planState)
{
	// When the stack is empty, find a goal task to push onto the task or add tasks as per usual
	// If any Decompositions or Preconditions fail, we must fail the entire plan because we have
	//  no alternate methods
	if (planState.DecompositionStack.empty())
	{
		if (planState.InitialCallList.empty())
			return PlanStepStatus::NoTasks;
		else if (planState.WorkingCallList.empty() && planState.FinalCallList.empty())
			planState.WorkingCallList = planState.InitialCallList;
		else if (planState.WorkingCallList.empty() && !planState.FinalCallList.empty())
			return PlanStepStatus::PlanComplete;

		TaskCallList compoundDecompositions;

		std::cout << "\nPlanStep()\nplanState.WorkingCallList.size() = "
		          << planState.WorkingCallList.size() << "\n";
		printTaskCallList(planState.WorkingCallList);

		for (TaskCallListIterator currentTaskCallIter = planState.WorkingCallList.begin();
		     currentTaskCallIter != planState.WorkingCallList.end();
		     currentTaskCallIter = planState.WorkingCallList.erase(currentTaskCallIter),
		                          std::cout << "Erased "
		                                    << "planState.WorkingCallList.size() = "
		                                    << planState.WorkingCallList.size() << "\n")
		{
			TaskCall currentTaskCall = (*currentTaskCallIter);
			Task* currentTask = currentTaskCall.TaskToCall;
			if (!currentTask)
				continue;
			TaskType currentTaskType = currentTask->GetType();
			// NullEntity = TODO
			TaskArguments taskArguments = {0, planState.StacklessState, currentTaskCall.Parameters};

			std::cout << "TaskType currentTaskType = " << (int)currentTaskType << "\n";

			switch (currentTaskType)
			{
				case TaskType::Goal:
				{
					std::cout << "Goal\n";
					GoalTask* goalTask = currentTask->Goal;
					GoalDecomposition decomposition;
					decomposition.DecomposedGoalTask = goalTask;
					decomposition.MethodIndex = 0;  // -- CHANGE for stacked
					// How the hell are parameters off of goals used? Fuck
					// Well, a method could be restricted to a single Primitive Task or a
					// CompoundTask
					decomposition.Parameters = currentTaskCall.Parameters;
					decomposition.InitialState = planState.StacklessState;  // -- CHANGE for stacked

					// Perform the decomposition
					if (!goalTask->DecomposeMethodAtIndex(decomposition.CallList,
					                                      decomposition.MethodIndex,
					                                      decomposition.Parameters))
						return PlanStepStatus::Failed_NoPossiblePlan;

					planState.DecompositionStack.push_back(decomposition);
					std::cout << "planState.DecompositionStack.push_back(decomposition);\n";

					if (planState.BreakOnStackPush)
					{
						planState.WorkingCallList.erase(currentTaskCallIter);
						return PlanStepStatus::SuccessfulDecomposition;
					}
					// TODO: IMPORTANT! This code will collapse horribly if I don't now start
					// processing the stack
					planState.WorkingCallList.erase(currentTaskCallIter);
					return PlanStepStatus::SuccessfulDecomposition;
				}
				break;
				case TaskType::Primitive:
				{
					std::cout << "Primitive\n";
					PrimitiveTask* primitiveTask = currentTask->Primitive;
					if (!primitiveTask->StateMeetsPreconditions(taskArguments))
						return PlanStepStatus::Failed_NoPossiblePlan;

					// If at top level, apply primitive tasks immediately to the stackless state
					primitiveTask->ApplyStateChange(taskArguments);
					planState.StacklessState = taskArguments.worldState;

					planState.FinalCallList.push_back(currentTaskCall);

					if (planState.BreakOnPrimitiveApply)
					{
						planState.WorkingCallList.erase(currentTaskCallIter);
						return PlanStepStatus::SuccessfulPrimitive;
					}
					// Keep processing tasks otherwise
				}
				break;
				case TaskType::Compound:
				{
					std::cout << "Compound\n";
					CompoundTask* compoundTask = currentTask->Compound;
					if (!compoundTask->StateMeetsPreconditions(taskArguments))
						return PlanStepStatus::Failed_NoPossiblePlan;

					if (!compoundTask->Decompose(compoundDecompositions, taskArguments))
						return PlanStepStatus::Failed_NoPossiblePlan;

					currentTaskCallIter = planState.WorkingCallList.erase(currentTaskCallIter);

					// we need to push our decomposition to our call list, but we're iterating on
					// 	it. Break out of the loop and tack it on there
				}
				break;
				default:
					return PlanStepStatus::Failed_BadData;
			}

			if (compoundDecompositions.size())
			{
				std::cout << "compoundDecompositions.size() = " << compoundDecompositions.size()
				          << "\n";
				planState.WorkingCallList.insert(planState.WorkingCallList.begin(),
				                                 compoundDecompositions.begin(),
				                                 compoundDecompositions.end());
				// We have to break here because we are adding things to the list we're iterating
				//  on; We'll process the tasks next Step
				// break; // UPDATE: Eventually we'll need a way to start over the loop if the
				// caller wants whole plan in one call
				std::cout << "PlanStep Done\n";
				return PlanStepStatus::SuccessfulDecomposition;
			}

			std::cout << "Loop Done\n";
		}
	}
	else
	{
		// Remember: If goal fails to decompose and goal is bottom of stack, fail
		GoalDecompositionStack::iterator currentStackFrameIter =
		    planState.DecompositionStack.end() - 1;
		GoalDecomposition& currentStackFrame = *currentStackFrameIter;

		std::cout << "\nPlanStep()\ncurrentStackFrame.CallList.size() = "
		          << currentStackFrame.CallList.size() << "\n";
		printTaskCallList(currentStackFrame.CallList);

		std::cout << "Stack Depth: ";
		for (int i = 0; i < planState.DecompositionStack.size(); i++)
			std::cout << "=";
		std::cout << "\n";

		{
			std::cout << "----Fullstack working lists\n";
			std::cout << "[0]\n";
			printTaskCallList(planState.WorkingCallList);
			int i = 1;
			for (GoalDecomposition& stackFrame : planState.DecompositionStack)
			{
				std::cout << "[" << i++ << "]\n";
				printTaskCallList(stackFrame.CallList);
			}
			std::cout << "----\n";
		}

		TaskCallList compoundDecompositions;
		bool methodFailed = false;

		for (TaskCallListIterator currentTaskCallIter = currentStackFrame.CallList.begin();
		     currentTaskCallIter != currentStackFrame.CallList.end();
		     currentTaskCallIter = currentStackFrame.CallList.erase(currentTaskCallIter),
		                          std::cout << "Erased "
		                                    << "currentStackFrame.CallList.size() = "
		                                    << planState.WorkingCallList.size() << "\n")
		{
			TaskCall currentTaskCall = (*currentTaskCallIter);
			Task* currentTask = currentTaskCall.TaskToCall;
			if (!currentTask)
				continue;
			TaskType currentTaskType = currentTask->GetType();
			// NullEntity = TODO
			TaskArguments taskArguments = {0, currentStackFrame.WorkingState,
			                               currentTaskCall.Parameters};

			std::cout << "TaskType currentTaskType = " << (int)currentTaskType << "\n";

			switch (currentTaskType)
			{
				case TaskType::Goal:
				{
					std::cout << "Goal\n";
					GoalTask* goalTask = currentTask->Goal;
					GoalDecomposition decomposition;
					decomposition.DecomposedGoalTask = goalTask;
					decomposition.MethodIndex = 0;
					// How the hell are parameters off of goals used? Fuck
					// Well, a method could be restricted to a single Primitive Task or a
					// CompoundTask
					decomposition.Parameters = currentTaskCall.Parameters;
					decomposition.InitialState = currentStackFrame.WorkingState;

					// Perform the decomposition
					if (!goalTask->DecomposeMethodAtIndex(decomposition.CallList,
					                                      decomposition.MethodIndex,
					                                      decomposition.Parameters))
						return PlanStepStatus::Failed_NoPossiblePlan;

					// TODO erase ahead of time because fuck
					// Strange things are afoot when we push to stack
					currentStackFrame.CallList.erase(currentTaskCallIter);

					{
						std::cout << "----Fullstack working lists [PRE PUSH BACK]\n";
						std::cout << "[0]\n";
						printTaskCallList(planState.WorkingCallList);
						int i = 1;
						for (GoalDecomposition& stackFrame : planState.DecompositionStack)
						{
							std::cout << "[" << i++ << "]\n";
							printTaskCallList(stackFrame.CallList);
						}
						std::cout << "----\n";
					}

					planState.DecompositionStack.push_back(decomposition);
					std::cout << "planState.DecompositionStack.push_back(decomposition);\n";

					// This code is wrong. I'm not sure why.
					// Pushing to the stack invalidates our currentStackFrame reference; update it
					// What the actual fuck (updating the ref doesn't fix the fucking thing)
					/*std::cout << "Ref updating from " << &currentStackFrame << " to "
					          << &(*(planState.DecompositionStack.end() - 1)) << "\n";
					currentStackFrameIter = planState.DecompositionStack.end() - 1;
					currentStackFrame = *currentStackFrameIter;*/

					{
						std::cout << "----Fullstack working lists [POST PUSH BACK]\n";
						std::cout << "[0]\n";
						printTaskCallList(planState.WorkingCallList);
						int i = 1;
						for (GoalDecomposition& stackFrame : planState.DecompositionStack)
						{
							std::cout << "[" << i++ << "]\n";
							printTaskCallList(stackFrame.CallList);
						}
						std::cout << "----\n";
					}

					if (planState.BreakOnStackPush)
					{
						// currentStackFrame.CallList.erase(currentTaskCallIter);
						return PlanStepStatus::SuccessfulDecomposition;
					}
					// TODO: IMPORTANT! This code will collapse horribly if I don't now start
					// processing the stack
					// currentStackFrame.CallList.erase(currentTaskCallIter);
					return PlanStepStatus::SuccessfulDecomposition;
				}
				break;
				case TaskType::Primitive:
				{
					std::cout << "Primitive\n";
					PrimitiveTask* primitiveTask = currentTask->Primitive;
					if (!primitiveTask->StateMeetsPreconditions(taskArguments))
					{
						methodFailed = true;
						break;
					}

					primitiveTask->ApplyStateChange(taskArguments);
					currentStackFrame.WorkingState = taskArguments.worldState;

					currentStackFrame.FinalCallList.push_back(currentTaskCall);

					if (planState.BreakOnPrimitiveApply)
					{
						currentStackFrame.CallList.erase(currentTaskCallIter);
						return PlanStepStatus::SuccessfulPrimitive;
					}
					// Keep processing tasks otherwise
				}
				break;
				case TaskType::Compound:
				{
					std::cout << "Compound\n";
					CompoundTask* compoundTask = currentTask->Compound;
					if (!compoundTask->StateMeetsPreconditions(taskArguments))
					{
						methodFailed = true;
						break;
					}

					if (!compoundTask->Decompose(compoundDecompositions, taskArguments))
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
					return PlanStepStatus::Failed_BadData;
			}

			if (methodFailed)
			{
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
					planState.DecompositionStack.pop_back();
					return PlanStepStatus::FailedGoalDecomposition;
				}

				// TODO: make this optional return
				return PlanStepStatus::FailedMethodDecomposition;
			}

			if (compoundDecompositions.size())
			{
				std::cout << "compoundDecompositions.size() = " << compoundDecompositions.size()
				          << "\n";
				std::cout << "currentStackFrame.CallList.size() = "
				          << currentStackFrame.CallList.size() << "\n";
				std::cout << "Decomposition:\n";
				printTaskCallList(compoundDecompositions);
				currentStackFrame.CallList.insert(currentStackFrame.CallList.begin(),
				                                  compoundDecompositions.begin(),
				                                  compoundDecompositions.end());
				// We have to break here because we are adding things to the list we're iterating
				//  on; We'll process the tasks next Step
				// break; // UPDATE: TODO: Eventually we'll need a way to start over the loop if the
				// caller wants whole plan in one call
				std::cout << "PlanStep Done\n";
				return PlanStepStatus::SuccessfulDecomposition;
			}

			std::cout << "Loop Done\n";
		}

		// Finished processing this stack frame
		if (currentStackFrame.CallList.empty() && !currentStackFrame.FinalCallList.empty())
		{
			bool onlyStackFrame = planState.DecompositionStack.size() == 1;
			TaskCallList* parentFinalCallList = nullptr;
			WorldState* parentWorkingState = nullptr;

			// If this is the only frame on the stack, its call list and working state goes to the
			// plan, else, the previous stack
			if (onlyStackFrame)
			{
				parentFinalCallList = &planState.FinalCallList;
				parentWorkingState = &planState.StacklessState;
			}
			else
			{
				GoalDecomposition& previousStackFrame = *(currentStackFrameIter - 1);

				parentFinalCallList = &previousStackFrame.FinalCallList;
				parentWorkingState = &previousStackFrame.WorkingState;
			}

			std::cout << "Collapsing stack frame. Adding List:\n";
			printTaskCallList(currentStackFrame.FinalCallList);
			std::cout << "To parent:\n";
			printTaskCallList(*parentFinalCallList);
			// Should this be appended?
			parentFinalCallList->insert(parentFinalCallList->end(),
			                            currentStackFrame.FinalCallList.begin(),
			                            currentStackFrame.FinalCallList.end());

			*parentWorkingState = currentStackFrame.WorkingState;

			planState.DecompositionStack.pop_back();

			std::cout << "Frame Done\n";

			if (planState.BreakOnStackPop)
				return PlanStepStatus::SuccessfulDecomposition;

			// TODO: Continue processing the stack
			return PlanStepStatus::SuccessfulDecomposition;
		}
	}

	return PlanStepStatus::PlanComplete;
}
}