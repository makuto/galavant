#include "HTNTasks.hpp"

#include <cassert>
#include <iostream>

namespace Htn
{
/*Task::Task(void)
{
    Type = TaskType::None;
}

TaskType Task::GetType(void)
{
    return Type;
}*/

int GoalTask::GetNumMethods(void)
{
	return Methods ? Methods->size() : 0;
}

bool GoalTask::DecomposeMethodAtIndex(TaskCallList& decomposition, int index,
                                      ParameterList& parameters)
{
	Task* methodToDecompose = GetMethodAtIndex(index);
	if (!methodToDecompose)
		return false;

	// TODO Fix this is screwy
	decomposition.push_back({methodToDecompose, parameters});
	return true;
}

Task* GoalTask::GetMethodAtIndex(int index)
{
	if (index < 0 || index >= GetNumMethods() || !Methods)
		return nullptr;

	return (*Methods)[index];
}

void GoalTask::SetMethods(TaskList* newMethods)
{
	Methods = newMethods;
}

CompoundTask::CompoundTask(void)
{
}

PrimitiveTask::PrimitiveTask(void)
{
}

CompoundTask::~CompoundTask(void)
{
}

PrimitiveTask::~PrimitiveTask(void)
{
}

Task::Task(GoalTask* goal)
{
	Goal = goal;
	Type = TaskType::Goal;
}

Task::Task(CompoundTask* compound)
{
	Compound = compound;
	Type = TaskType::Compound;
}

Task::Task(PrimitiveTask* primitive)
{
	Primitive = primitive;
	Type = TaskType::Primitive;
}

TaskType Task::GetType(void) const
{
	return Type;
}

GoalTask* Task::GetGoal(void)
{
	assert(Type == TaskType::Goal);
	return Goal;
}
CompoundTask* Task::GetCompound(void)
{
	assert(Type == TaskType::Compound);
	return Compound;
}
PrimitiveTask* Task::GetPrimitive(void)
{
	assert(Type == TaskType::Primitive);
	return Primitive;
}

std::ostream& operator<<(std::ostream& os, const Task& task)
{
	switch (task.GetType())
	{
		case TaskType::None:
			os << "Task Type:None " << &task;
			break;
		case TaskType::Goal:
			os << "Task Type:Goal task addr " << &task << " Goal addr " << task.Goal;
			break;
		case TaskType::Compound:
			os << "Task Type:Compound addr " << &task << " Compound addr " << task.Compound;
			break;
		case TaskType::Primitive:
			os << "Task Type:Primitive addr " << &task << " Primitive addr " << task.Primitive;
			break;
	}
	return os;
}

void printTaskList(const TaskList& tasks)
{
	std::cout << "TaskList size = " << tasks.size() << " addr " << &tasks << ":\n";
	for (unsigned int i = 0; i < tasks.size(); i++)
	{
		std::cout << "\t[" << i << "] " << *tasks[i] << "\n";
	}
}

void printTaskCallList(const TaskCallList& tasks)
{
	std::cout << "TaskCallList size = " << tasks.size() << " addr " << &tasks << ":\n";
	for (unsigned int i = 0; i < tasks.size(); i++)
	{
		std::cout << "\t[" << i << "] " << *tasks[i].TaskToCall << "\n";
	}
}
}