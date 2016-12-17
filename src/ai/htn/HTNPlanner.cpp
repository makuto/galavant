#include "HTNTypes.hpp"
#include "HTNTasks.hpp"

namespace Htn
{
bool DecomposeGoalTask(TaskList& taskList, Task* task, const TaskArguments& taskArguments)
{
	GoalTask* goalTask = static_cast<GoalTask*>(task);
	return false;  // TODO
}

bool DecomposeCompoundTask(TaskList& taskList, Task* task, const TaskArguments& taskArguments)
{
	CompoundTask* compoundTask = static_cast<CompoundTask*>(task);

	if (compoundTask->StateMeetsPreconditions(taskArguments))
		// Decomposition is determined by the task itself
		return compoundTask->Decompose(taskList, taskArguments);

	return false;
}

struct DecomposeTaskState
{
	Task* DecomposedTask;
	ParameterList Parameters;
};

typedef std::vector<DecomposeTaskState> DecomposedTaskList;

struct TaskDecomposition
{
	Task* DecomposedTask;
	unsigned int DecompositionIndex;
	DecomposedTaskList* Decomposition;
};

typedef std::vector<TaskDecomposition> TaskDecompositionStack;

enum class PlanStepStatus
{
	Failed = 0,  // generic failure

	FailedDecomposition,
	SuccessfulDecomposition,

	FailedToMeetPrecondtions,
	StateMeetsPreconditions,

	NoTasks,

	PlanComplete
};

/*
Goal Task // Set Agent StateA and StateB = true
	Method1 = Compound1(AgentState and Params from initial call)
	Method2 = Compound2
	Method3 = Compound3

Compound1
	StateMeetsPrecondtion(AgentState, Params from Goal) then 
		return {primitiveA(paramA, paramB), primitiveB(ParamA, ParamB, ParamC)}

primitiveA
	StateMeetsPrecondition(AgentState, paramA, paramB)
		then AgentState.StateA = true

primitiveB
	StateMeetsPrecondition(AgentState, paramA, paramB, paramC)
		then AgentState.StateB = true

PlanStep(stack, taskList)
if not stack
 	// startup
	stack.push(decompose(taskList[0]))

else
	decomposition = stack.pop()
	allPrimitives = true
	for task in decomposition
		if task == primitive
			task.StateMeetsPrecondition then
				modify currentState
				// don't break here
			else
				goto fail

		if task == compound
			allPrimitives = false
			task.StateMeetsPrecondition then
				stack.push(decompose(task))
				break
			else
				goto fail

		if task == goal
			allPrimitives = false
			methodIndex = 0
			decomposedTasks = task.GetMethodAtIndex(0), currentState
			stack.push(decomposition)
			break

	fail:
		

*/

PlanStepStatus PlanStep(TaskDecompositionStack& stack, TaskList& taskList,
                        const TaskArguments& taskArguments)
{
	if (!stack.empty())
	{
		// Check to see if the current stack item is all primitive tasks with met preconditions. If
		// 	so, we can add them to the tasklist and walk all the way back up
		// TODO: naming
		TaskDecomposition currentDecomposition = stack.end();
		bool onlyPrimitiveTasksRemain = true;
		bool failedToDecompose = false;
		for (DecomposeTaskState* currentTaskState : currentDecomposition.Decomposition)
		{
			if (!currentTaskState || !currentTaskState->DecomposeTask)
				continue;
			Task* currentTask = currentTaskState->DecomposeTask;
			TaskType currentTaskType = currentTask->GetType();
			TaskArguments& currentTaskArguments = currentDecomposition->Arguments;
			
			if (currentTaskType == TaskType::Primitive) 
			{
				PrimitiveTask* primitiveTask = static_cast<PrimitiveTask*>(currentTask);

				if (!primitiveTask->StateMeetsPreconditions(currentTaskArguments))
				{
					failedToDecompose = true;
					break;
				}

				// Modify state

			}
			else
			{
				onlyPrimitiveTasksRemain = false;
				// TODO
				//failedToDecompose = DecomposeTask()
			}
		}

		// TODO: Goal tasks trying new decompositions
		if (failedToDecompose)
		{
			stack.pop_back();
			delete currentDecomposition;
			return PlanStepStatus::FailedDecomposition;
		}
	}
	else if (!taskList.empty())
	{
		// No stack; we must be just starting
		Task* currentTask = *taskList.data();

		if (!currentTask)
			return PlanStepStatus::Failed;

		TaskType currentTaskType = currentTask->GetType();

		// What if taskList just has primitives? Multiple goals?
		if (currentTaskType == TaskType::Goal)
		{
			GoalTask* goalTask = static_cast<GoalTask*>(currentTask);
			int numMethods = goalTask->GetNumMethods();
			TaskDecomposition newDecomposition;
			newDecomposition.DecomposedTask = currentTask;
			newDecomposition.DecompositionIndex = 0;
			newDecomposition.Decomposition = nullptr;

			// Goal tasks are decomposed via methods. Methods are Tasks which can be used to achieve
			//  the goal. Goal tasks do not have conditonals - they just blindly pick methods in
			//  order until they find one which works
			for (int methodIndex = newDecomposition.DecompositionIndex;
			     !newDecomposition.Decomposition && methodIndex < numMethods; methodIndex++)
			{
				Task* method = goalTask->GetMethodAtIndex(newDecomposition.DecompositionIndex);
				if (!method)
					continue;
				TaskType methodType = method->GetType();

				if (methodType == TaskType::GoalTask)
				{
					newDecomposition.newDecomposition = new TaskList(1);
					newDecomposition.Decomposition->push_back(method);
					newDecomposition.DecompositionIndex = methodIndex;
				}
				else if (methodType == TaskType::Primitive)
				{
					PrimitiveTask* primitiveTask = static_cast<PrimitiveTask*>(method);
					if (primitiveTask->StateMeetsPreconditions(taskArguments))
					{
						newDecomposition.newDecomposition = new TaskList(1);
						newDecomposition.Decomposition->push_back(method);
						newDecomposition.DecompositionIndex = methodIndex;
					}
				}
				else if (methodType == TaskType::CompoundTask)
				{
					CompoundTask* compoundTask = static_cast<CompoundTask*>(method);
					if (compoundTask->StateMeetsPreconditions(taskArguments))
					{
						newDecomposition.newDecomposition = new TaskList();
						compoundTask->Decompose(&newDecomposition.newDecomposition, taskArguments);
						newDecomposition.DecompositionIndex = methodIndex;
					}
				}
				else
					// We got something we don't know how to handle
					return PlanStepStatus::Failed;
			}

			stack.push_back(newDecomposition);
			return PlanStepStatus::SuccessfulDecomposition;
		}
	}
	else
		return PlanStepStatus::NoTasks;
}

/*PlanStepStatus PlanStep(TaskDecompositionStack& stack, TaskList& taskList,
                        const TaskArguments& taskArguments)
{
    bool planComplete = true;

    // Check to see if there are tasks which need to be decomposed
    for (Task* currentTask : taskList)
    {
        if (!currentTask)
            continue;

        TaskType currentTaskType = currentTask->GetType();

        // The plan is complete once all Goal and Compound Tasks have been decomposed into Primitive
        // Tasks, which we can actually execute
        if (currentTaskType != TaskType::Primitive)
        {
            planComplete = false;
            break;
        }
    }

    if (!planComplete)
    {
        Task* currentTask = *taskList.data();

        if (!currentTask)
            return PlanStepStatus::Failed;

        TaskType currentTaskType = currentTask->GetType();

        if (currentTaskType == TaskType::Goal)
        {
            GoalTask* goalTask = static_cast<GoalTask*>(currentTask);

        }
    }
    else
        return PlanStepStatus::PlanComplete;
}*/

/*// Take the first task in the list and decompose it or make sure it's a valid part of the plan
PlanStepStatus PlanStep(TaskDecompositionStack& stack, TaskList& taskList,
                        const TaskArguments& taskArguments)
{
    if (taskList.size())
    {
        Task* currentTask = *taskList.data();
        TaskType taskType = currentTask->GetType();

        // If it's a goal, we want to find a compound task that will accomplish it
        if (taskType == TaskType::Goal)
        {
            if (DecomposeGoalTask(taskList, currentTask, taskArguments))
                return PlanStepStatus::SuccessfulDecomposition;
            else
                return PlanStepStatus::FailedDecomposition;
        }

        // If it's a compound task, we want to decompose it
        if (taskType == TaskType::Compound)
        {
            if (DecomposeCompoundTask(taskList, currentTask, taskArguments))
                return PlanStepStatus::SuccessfulDecomposition;
            else
                return PlanStepStatus::FailedDecomposition;
        }

        // If it's a primitive task, we want to make sure we can actually execute it with our given
        // state
        if (taskType == TaskType::Primitive)
        {
            // TODO
        }

        return PlanStepStatus::Failed;
    }

    return PlanStepStatus::NoTasks;
}*/
}