#include <iostream>

#include "../ai/htn/HTNTypes.hpp"
#include "../ai/htn/HTNTasks.hpp"
#include "../ai/htn/HTNPlanner.hpp"

class TestPrimitiveTask : public Htn::PrimitiveTask
{
public:
	TestPrimitiveTask(void)
	{
		// Type = Htn::TaskType::Primitive;
	}

	virtual ~TestPrimitiveTask(void)
	{
	}

	virtual bool StateMeetsPreconditions(const Htn::TaskArguments& args)
	{
		std::cout << "\tStateMeetsPreconditions TestPrimitiveTask\n";
		return true;
	}

	virtual void ApplyStateChange(Htn::TaskArguments& arguments)
	{
		std::cout << "\tApplyStateChange TestPrimitiveTask\n";
	}

	virtual bool Execute(const Htn::TaskArguments& args)
	{
		std::cout << "\texecute TestPrimitiveTask: " << args.Parameters[0].IntValue << "\n";
		return true;
	}
};

class TestCompoundTaskA : public Htn::CompoundTask
{
public:
	TestCompoundTaskA(void)
	{
		// Type = Htn::TaskType::Primitive;
	}

	virtual ~TestCompoundTaskA(void)
	{
	}

	virtual bool StateMeetsPreconditions(const Htn::TaskArguments& args)
	{
		std::cout << "\tStateMeetsPreconditions TestCompoundTaskA\n";
		return true;
	}

	virtual bool Decompose(Htn::TaskCallList& taskCallList, const Htn::TaskArguments& args)
	{
		static TestPrimitiveTask testPrimitiveTask;
		std::cout << "\tDecompose TestCompoundTaskA: " << args.Parameters[0].IntValue << "\n";
		static Htn::Task primitiveTask;
		primitiveTask.Primitive = &testPrimitiveTask;
		primitiveTask.Goal = nullptr;
		Htn::TaskCall taskCall = {&primitiveTask, args.Parameters};
		taskCallList.push_back(taskCall);
		return true;
	}
};

int main()
{
	// Test parameters
	Htn::Parameter testParam = {Htn::Parameter::ParamType::Int, 123};
	std::cout << testParam.FloatValue << "\n";

	// Compound task setup
	TestCompoundTaskA testCompoundTaskAA;

	Htn::Task compoundTask;
	compoundTask.Compound = &testCompoundTaskAA;
	Htn::ParameterList params;
	params.push_back(testParam);
	Htn::TaskCall taskCall = {&compoundTask, params};

	// Goal task setup (single)
	Htn::GoalTask goalTask;
	Htn::TaskList methods;
	methods.push_back(&compoundTask);
	goalTask.SetMethods(&methods);
	Htn::Task goalTaskTask;
	goalTaskTask.Goal = &goalTask;
	Htn::TaskCall goalTaskCall = {&goalTaskTask, params};

	// Goal task setup (nested)
	Htn::GoalTask nestedGoalTask;
	Htn::TaskList nestedMethods;
	nestedMethods.push_back(&goalTaskTask);
	nestedGoalTask.SetMethods(&nestedMethods);
	Htn::Task nestedGoalTaskTask;
	nestedGoalTaskTask.Goal = &nestedGoalTask;
	Htn::TaskCall nestedGoalTaskCall = {&nestedGoalTaskTask, params};

	// Compounds and primitives, but no goals
	{
		Htn::PlanState testPlan;
		testPlan.InitialCallList.push_back(taskCall);
		testPlan.InitialCallList.push_back(taskCall);
		Htn::TaskArguments args;
		args.Parameters = params;
		testCompoundTaskAA.Decompose(testPlan.InitialCallList, args);

		for (int i = 0; i < 10; i++)
		{
			Htn::PlanStepStatus status = Htn::PlanStep(testPlan);
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::PlanStepStatus::PlanComplete)
				break;
		}

		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size()
		          << (testPlan.FinalCallList.size() == 3 ? " (Pass)" : " (Fail)") << "\n\n";
	}

	// One goal (one stack frame)
	{
		Htn::PlanState testPlan;
		testPlan.InitialCallList.push_back(goalTaskCall);

		for (int i = 0; i < 10; i++)
		{
			Htn::PlanStepStatus status = Htn::PlanStep(testPlan);
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::PlanStepStatus::PlanComplete)
				break;
		}

		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size()
		          << (testPlan.FinalCallList.size() == 1 ? " (Pass)" : " (Fail)") << "\n\n";
	}

	// Nested goal (two stack frames)
	{
		Htn::PlanState testPlan;
		testPlan.InitialCallList.push_back(nestedGoalTaskCall);
		testPlan.InitialCallList.push_back(nestedGoalTaskCall);

		for (int i = 0; i < 12; i++)
		{
			Htn::PlanStepStatus status = Htn::PlanStep(testPlan);
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::PlanStepStatus::PlanComplete)
				break;
		}

		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size()
		          << (testPlan.FinalCallList.size() == 2 ? " (Pass)" : " (Fail)") << "\n\n";
		printTaskCallList(testPlan.FinalCallList);
	}

	return 0;
}