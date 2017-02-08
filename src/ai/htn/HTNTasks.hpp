#pragma once

#include "HTNTypes.hpp"

#include <iostream>

namespace Htn
{
struct Task;
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

public:
	GoalTask(void) = default;

	int GetNumMethods(void);
	Task* GetMethodAtIndex(int index);

	bool DecomposeMethodAtIndex(TaskCallList& decomposition, int index,
	                            const ParameterList& parameters);

	void SetMethods(TaskList* newMethods);
};

class CompoundTask
{
public:
	CompoundTask(void) = default;
	virtual ~CompoundTask(void) = default;
	virtual bool StateMeetsPreconditions(const WorldState& state,
	                                     const ParameterList& parameters) const = 0;
	virtual bool Decompose(TaskCallList& taskCallList, const WorldState& state,
	                       const ParameterList& parameters) = 0;
};

class PrimitiveTask
{
public:
	PrimitiveTask(void) = default;
	virtual ~PrimitiveTask(void) = default;
	virtual bool StateMeetsPreconditions(const WorldState& state,
	                                     const ParameterList& parameters) const = 0;
	virtual void ApplyStateChange(WorldState& state, const ParameterList& parameters) = 0;
	// Returns whether or not starting the task was successful (NOT whether the task completed)
	// Execution should (when completed etc.) modify the world state the same as ApplyStateChange
	// would. Call that function for extra safety
	virtual bool Execute(WorldState& state, const ParameterList& parameters) = 0;
};

enum class TaskType
{
	None = 0,
	Goal,
	Compound,
	Primitive
};

// Instead of using inheritance, just use a simple struct which stores all types of tasks but
//  only allow only one thing to be filled in for it
struct Task
{
	Task(void) = delete;
	Task(GoalTask* goal);
	Task(CompoundTask* compound);
	Task(PrimitiveTask* primitive);

	TaskType GetType(void) const;

	GoalTask* GetGoal(void);
	CompoundTask* GetCompound(void);
	PrimitiveTask* GetPrimitive(void);

	friend std::ostream& operator<<(std::ostream& os, const Task& task);

private:
	union
	{
		GoalTask* Goal;
		CompoundTask* Compound;
		PrimitiveTask* Primitive;
	};

	TaskType Type;
};

std::ostream& operator<<(std::ostream& os, const Task& task);

void printTaskList(const TaskList& tasks);
void printTaskCallList(const TaskCallList& tasks);
}