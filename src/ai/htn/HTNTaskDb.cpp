#include "HTNTaskDb.hpp"

#include "../../util/Logging.hpp"

namespace Htn
{
namespace TaskDb
{
typedef std::map<TaskName, Task*> TaskDb;
static TaskDb s_TaskDb;
void ClearAllTasks()
{
	s_TaskDb.clear();
}

void AddTask(Task* task, TaskName taskName)
{
	if (task && taskName != TaskName::None)
	{
		TaskDb::iterator findIt = s_TaskDb.find(taskName);
		if (findIt != s_TaskDb.end())
			LOGW << "Replacing task with name " << (int)taskName << ". If in Unreal, this is fine";

		s_TaskDb[taskName] = task;
	}
}

Task* GetTask(TaskName taskName)
{
	if (taskName != TaskName::None)
	{
		TaskDb::iterator findIt = s_TaskDb.find(taskName);
		if (findIt != s_TaskDb.end())
			return findIt->second;
	}
	return nullptr;
}
}
}