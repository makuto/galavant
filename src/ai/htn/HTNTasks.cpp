#include "HTNTasks.hpp"

namespace Htn
{
Task::Task(void)
{
	Type = TaskType::None;
}

TaskType Task::GetType(void)
{
	return Type;
}

int GoalTask::GetNumMethods(void)
{
	return Methods ? Methods->size() : 0;
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
	Type = TaskType::Compound;
}

PrimitiveTask::PrimitiveTask(void)
{
	Type = TaskType::Primitive;
}
}