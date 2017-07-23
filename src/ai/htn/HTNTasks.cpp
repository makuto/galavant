#include "HTNTasks.hpp"

#include <cassert>

#include "../../util/Logging.hpp"

namespace Htn
{
int GoalTask::GetNumMethods()
{
	return Methods ? Methods->size() : 0;
}

bool GoalTask::DecomposeMethodAtIndex(TaskCallList& decomposition, int index,
                                      const ParameterList& parameters)
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

Task* GoalTask::GetTask()
{
	if (TaskContainer.GetType() == TaskType::None)
		TaskContainer.Initialize(this);
	return &TaskContainer;
}

Task* PrimitiveTask::GetTask()
{
	if (TaskContainer.GetType() == TaskType::None)
		TaskContainer.Initialize(this);
	return &TaskContainer;
}

Task* CompoundTask::GetTask()
{
	if (TaskContainer.GetType() == TaskType::None)
		TaskContainer.Initialize(this);
	return &TaskContainer;
}

Task::Task(GoalTask* goal)
{
	Initialize(goal);
}

Task::Task(CompoundTask* compound)
{
	Initialize(compound);
}

Task::Task(PrimitiveTask* primitive)
{
	Initialize(primitive);
}

void Task::Initialize(GoalTask* goal)
{
	Goal = goal;
	Type = TaskType::Goal;
}

void Task::Initialize(CompoundTask* compound)
{
	Compound = compound;
	Type = TaskType::Compound;
}

void Task::Initialize(PrimitiveTask* primitive)
{
	Primitive = primitive;
	Type = TaskType::Primitive;
}

TaskType Task::GetType() const
{
	return Type;
}

GoalTask* Task::GetGoal()
{
	assert(Type == TaskType::Goal);
	return Goal;
}
CompoundTask* Task::GetCompound()
{
	assert(Type == TaskType::Compound);
	return Compound;
}
PrimitiveTask* Task::GetPrimitive()
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

void PrintTaskList(const TaskList& tasks)
{
	LOGD << "TaskList size = " << tasks.size() << " addr " << &tasks << ":";
	for (unsigned int i = 0; i < tasks.size(); i++)
		LOGD << "    [" << i << "] " << *tasks[i];
}

void PrintTaskCallList(const TaskCallList& tasks)
{
	LOGD << "TaskCallList size = " << tasks.size() << " addr " << &tasks << ":";
	for (unsigned int i = 0; i < tasks.size(); i++)
		LOGD << "    [" << i << "] " << *tasks[i].TaskToCall;
}
}

template <>
gv::Logging::Record& gv::Logging::Record::operator<<<Htn::Task>(const Htn::Task& task)
{
	switch (task.GetType())
	{
		case Htn::TaskType::None:
			*this << "Task Type:None " << &task;
			break;
		case Htn::TaskType::Goal:
			*this << "Task Type:Goal task addr " << &task << " Goal";  // << task.Goal;
			break;
		case Htn::TaskType::Compound:
			*this << "Task Type:Compound addr " << &task << " Compound";  // << task.Compound;
			break;
		case Htn::TaskType::Primitive:
			*this << "Task Type:Primitive addr " << &task << " Primitive";  // << task.Primitive;
			break;
	}
	return *this;
}