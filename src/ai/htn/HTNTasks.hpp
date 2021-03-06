#pragma once

#include "HTNTypes.hpp"
#include "ai/WorldState.hpp"
#include "util/CallbackContainer.hpp"
#include "util/ResourceDictionary.hpp"

namespace Htn
{
enum class TaskType
{
	None = 0,
	Goal,
	Compound,
	Primitive
};

// Instead of using inheritance, just use a simple struct which stores all types of tasks but
//  only allow only one thing to be filled in for it
class GoalTask;
class CompoundTask;
class PrimitiveTask;
struct Task
{
	Task() = default;
	Task(GoalTask* goal);
	Task(CompoundTask* compound);
	Task(PrimitiveTask* primitive);

	void Initialize(GoalTask* goal);
	void Initialize(CompoundTask* compound);
	void Initialize(PrimitiveTask* primitive);

	TaskType GetType() const;

	GoalTask* GetGoal();
	CompoundTask* GetCompound();
	PrimitiveTask* GetPrimitive();

private:
	union
	{
		GoalTask* Goal;
		CompoundTask* Compound;
		PrimitiveTask* Primitive;
	};

	TaskType Type = TaskType::None;
};

typedef std::vector<Task*> TaskList;

struct TaskCall
{
	Task* TaskToCall;
	ParameterList Parameters;
};

typedef std::vector<TaskCall> TaskCallList;
typedef TaskCallList::iterator TaskCallListIterator;

//
// Tasks
//

class GoalTask
{
private:
	TaskList* Methods;
	Task TaskContainer;

public:
	GoalTask() = default;

	int GetNumMethods();
	Task* GetMethodAtIndex(int index);

	bool DecomposeMethodAtIndex(TaskCallList& decomposition, int index,
	                            const ParameterList& parameters);

	void SetMethods(TaskList* newMethods);

	Task* GetTask();
};

class CompoundTask
{
private:
	Task TaskContainer;

public:
	CompoundTask() = default;
	virtual ~CompoundTask() = default;
	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const ParameterList& parameters) const = 0;
	virtual bool Decompose(TaskCallList& taskCallList, const gv::WorldState& state,
	                       const ParameterList& parameters) = 0;

	Task* GetTask();
};

struct TaskExecuteStatus
{
	enum ExecutionStatus
	{
		Failed = 0,
		Succeeded,
		Running,
		WaitForEvent,
		Reexecute
	};

	ExecutionStatus Status;
};

class PrimitiveTask
{
private:
	Task TaskContainer;

public:
	PrimitiveTask() = default;
	virtual ~PrimitiveTask() = default;
	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const ParameterList& parameters) const = 0;
	virtual void ApplyStateChange(gv::WorldState& state, const ParameterList& parameters) = 0;
	// Returns whether or not starting the task was successful (NOT whether the task completed)
	// Execution should (when completed etc.) modify the world state the same as ApplyStateChange
	// would. Call that function for extra safety
	virtual TaskExecuteStatus Execute(gv::WorldState& state, const ParameterList& parameters) = 0;

	Task* GetTask();
};

void PrintTaskList(const TaskList& tasks);
void PrintTaskCallList(const TaskCallList& tasks);

extern gv::ResourceDictionary<Task> g_TaskDictionary;
}

template <>
gv::Logging::Record& gv::Logging::Record::operator<<<Htn::Task>(const Htn::Task& task);