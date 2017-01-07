#include "HTNTasks.hpp"

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

bool GoalTask::DecomposeMethodAtIndex(TaskCallList& decomposition, int index, ParameterList& parameters)
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
	// Type = TaskType::Compound;
}

PrimitiveTask::PrimitiveTask(void)
{
	// Type = TaskType::Primitive;
}

CompoundTask::~CompoundTask(void)
{
}

PrimitiveTask::~PrimitiveTask(void)
{
}

TaskType Task::GetType(void)
{
	if (Goal)
		return TaskType::Goal;
	if (Compound)
		return TaskType::Compound;
	if (Primitive)
		return TaskType::Primitive;
	return TaskType::None;
}
}