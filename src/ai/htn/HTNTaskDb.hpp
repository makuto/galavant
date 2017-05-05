#pragma once

#include "HTNTasks.hpp"

namespace Htn
{
enum class TaskName
{
	None = 0,

	// Goals

	// Compounds
	GetResource,

	// Primitives
	FindResource,
	MoveTo,
	InteractPickup,

	TaskName_count
};

namespace TaskDb
{
void ClearAllTasks();
void AddTask(Task* task, TaskName taskName);
Task* GetTask(TaskName taskName);
}
}