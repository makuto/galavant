#pragma once

#include "HTNTypes.hpp"

namespace Htn
{
/*enum class TaskType
{
	None = 0,
	Goal,
	Compound,
	Primitive
};

// Task only exists so that we can store all our Task types in a flat list
class Task
{
public:
	Task(void);

	// Type is used to know what to cast this Task to
	TaskType GetType(void);

protected:
	// This is set by the constructors of each different Task
	TaskType Type;
};*/

// Instead of the commented code, just use a simple struct which stores all types of tasks but 
//  only hope to allow only one thing to be filled in for it
struct Task
{
	GoalTask* Goal;
	CompoundTask* Compound;
	PrimitiveTask* Primitive;
};

typedef std::vector<Task*> TaskList;

//
// Different types of tasks
//

class GoalTask //: public Task
{
private:
	TaskList* Methods;

public:
	GoalTask(void);

	int GetNumMethods(void);
	Task* GetMethodAtIndex(int index);

	void SetMethods(TaskList* newMethods);
};

class CompoundTask //: public Task
{
public:
	CompoundTask(void);
	virtual bool StateMeetsPreconditions(const TaskArguments& arguments) = 0;
	virtual bool Decompose(TaskList& taskList, const TaskArguments& arguments) = 0;
};

class PrimitiveTask : public Task
{
public:
	PrimitiveTask(void);
	virtual bool StateMeetsPreconditions(const TaskArguments& arguments) = 0;
	virtual void ApplyStateChange(TaskArguments& arguments) = 0;
	// Returns whether or not starting the task was successful (NOT whether the task completed)
	virtual bool Execute(const TaskArguments& arguments) = 0;
};
}