#include <iostream>

#define CATCH_CONFIG_MAIN
#include "../../thirdParty/Catch/single_include/catch.hpp"

#include "util/Logging.hpp"

#include "ai/htn/HTNTypes.hpp"
#include "ai/htn/HTNTasks.hpp"
#include "ai/htn/HTNPlanner.hpp"

static gv::Logging::Logger s_logger;

class AlwaysFailPrimitiveTask : public Htn::PrimitiveTask
{
public:
	AlwaysFailPrimitiveTask() = default;
	virtual ~AlwaysFailPrimitiveTask() = default;

	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const Htn::ParameterList& parameters) const
	{
		std::cout << "\tStateMeetsPreconditions AlwaysFailPrimitiveTask\n";
		return false;
	}

	virtual void ApplyStateChange(gv::WorldState& state, const Htn::ParameterList& parameters)
	{
		std::cout << "\tApplyStateChange AlwaysFailPrimitiveTask\n";
	}

	virtual Htn::TaskExecuteStatus Execute(gv::WorldState& state,
	                                      const Htn::ParameterList& parameters)
	{
		std::cout << "\texecute AlwaysFailPrimitiveTask: " << parameters[0].IntValue << "\n";
		Htn::TaskExecuteStatus status {Htn::TaskExecuteStatus::ExecutionStatus::Failed};
		return status;
	}
};

class RequiresStatePrimitiveTask : public Htn::PrimitiveTask
{
public:
	RequiresStatePrimitiveTask() = default;
	virtual ~RequiresStatePrimitiveTask() = default;

	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const Htn::ParameterList& parameters) const
	{
		std::cout << "\tStateMeetsPreconditions RequiresStatePrimitiveTask state.TestStateChange = "
		          << state.TestStateChange << "\n";
		return state.TestStateChange == 1;
	}

	virtual void ApplyStateChange(gv::WorldState& state, const Htn::ParameterList& parameters)
	{
		std::cout << "\tApplyStateChange RequiresStatePrimitiveTask\n";
	}

	virtual Htn::TaskExecuteStatus Execute(gv::WorldState& state,
	                                      const Htn::ParameterList& parameters)
	{
		std::cout << "\texecute RequiresStatePrimitiveTask: " << parameters[0].IntValue << "\n";
		Htn::TaskExecuteStatus status {Htn::TaskExecuteStatus::ExecutionStatus::Succeeded};
		return status;
	}
};

class TestPrimitiveTask : public Htn::PrimitiveTask
{
public:
	TestPrimitiveTask() = default;
	virtual ~TestPrimitiveTask() = default;

	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const Htn::ParameterList& parameters) const
	{
		std::cout << "\tStateMeetsPreconditions TestPrimitiveTask\n";
		return true;
	}

	virtual void ApplyStateChange(gv::WorldState& state, const Htn::ParameterList& parameters)
	{
		std::cout << "\tApplyStateChange TestPrimitiveTask\n";
		state.TestStateChange = 1;
	}

	virtual Htn::TaskExecuteStatus Execute(gv::WorldState& state,
	                                      const Htn::ParameterList& parameters)
	{
		std::cout << "\texecute TestPrimitiveTask: " << parameters[0].IntValue << "\n";
		Htn::TaskExecuteStatus status {Htn::TaskExecuteStatus::ExecutionStatus::Succeeded};
		return status;
	}
};

class TestCompoundTaskA : public Htn::CompoundTask
{
public:
	TestCompoundTaskA() = default;

	virtual ~TestCompoundTaskA() = default;

	virtual bool StateMeetsPreconditions(const gv::WorldState& state,
	                                     const Htn::ParameterList& parameters) const
	{
		std::cout << "\tStateMeetsPreconditions TestCompoundTaskA\n";
		return true;
	}

	virtual bool Decompose(Htn::TaskCallList& taskCallList, const gv::WorldState& state,
	                       const Htn::ParameterList& parameters)
	{
		static TestPrimitiveTask testPrimitiveTask;
		static Htn::Task primitiveTask(&testPrimitiveTask);
		std::cout << "\tDecompose TestCompoundTaskA: " << parameters[0].IntValue << "\n";
		Htn::TaskCall taskCall = {&primitiveTask, parameters};
		taskCallList.push_back(taskCall);
		return true;
	}
};

TEST_CASE("Hierarchical Task Networks Planner")
{
	gv::InitializeConsoleOnlyLogging();

	Htn::Parameter testParam;
	testParam.Type = Htn::Parameter::ParamType::Int;
	testParam.IntValue = 123;
	Htn::ParameterList params;
	params.push_back(testParam);

	// Base tasks setup
	AlwaysFailPrimitiveTask failTask;
	RequiresStatePrimitiveTask requiresStateTask;
	TestCompoundTaskA testCompoundTaskAA;

	Htn::Task failTaskTask(&failTask);

	Htn::Task requiresStateTaskTask(&requiresStateTask);
	Htn::TaskCall requiresStateTaskCall = {&requiresStateTaskTask, params};

	Htn::Task compoundTask(&testCompoundTaskAA);
	Htn::TaskCall taskCall = {&compoundTask, params};

	// Goal task setup (single)
	Htn::GoalTask goalTask;
	Htn::TaskList methods;
	methods.push_back(&compoundTask);
	goalTask.SetMethods(&methods);
	Htn::Task goalTaskTask(&goalTask);
	Htn::TaskCall goalTaskCall = {&goalTaskTask, params};

	// Goal task setup (nested)
	Htn::GoalTask nestedGoalTask;
	Htn::TaskList nestedMethods;
	nestedMethods.push_back(&goalTaskTask);
	nestedGoalTask.SetMethods(&nestedMethods);
	Htn::Task nestedGoalTaskTask(&nestedGoalTask);
	Htn::TaskCall nestedGoalTaskCall = {&nestedGoalTaskTask, params};

	SECTION("Compounds and primitives, but no goals")
	{
		std::cout << "TEST: Compounds and primitives, but no goals\n\n";
		Htn::Planner testPlan;
		testPlan.InitialCallList.push_back(taskCall);
		testPlan.InitialCallList.push_back(taskCall);
		gv::WorldState nullState;
		testCompoundTaskAA.Decompose(testPlan.InitialCallList, nullState, params);

		Htn::Planner::Status status;
		for (int i = 0; i < 10; i++)
		{
			status = testPlan.PlanStep();
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::Planner::Status::PlanComplete)
				break;
		}

		REQUIRE(status == Htn::Planner::Status::PlanComplete);
		REQUIRE(testPlan.FinalCallList.size() == 3);
		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size() << "\n\n";
	}

	SECTION("One goal (one stack frame)")
	{
		std::cout << "TEST: One goal (one stack frame)\n\n";
		Htn::Planner testPlan;
		testPlan.InitialCallList.push_back(goalTaskCall);

		Htn::Planner::Status status;
		for (int i = 0; i < 10; i++)
		{
			status = testPlan.PlanStep();
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::Planner::Status::PlanComplete)
				break;
		}

		REQUIRE(status == Htn::Planner::Status::PlanComplete);
		REQUIRE(testPlan.FinalCallList.size() == 1);
		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size() << "\n";
		PrintTaskCallList(testPlan.FinalCallList);
		std::cout << "\n\n";
	}

	SECTION("Nested goal (two stack frames)")
	{
		std::cout << "TEST: Nested goal (two stack frames)\n\n";
		Htn::Planner testPlan;
		testPlan.InitialCallList.push_back(nestedGoalTaskCall);
		testPlan.InitialCallList.push_back(nestedGoalTaskCall);

		Htn::Planner::Status status;
		for (int i = 0; i < 12; i++)
		{
			status = testPlan.PlanStep();
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::Planner::Status::PlanComplete)
				break;
		}

		REQUIRE(status == Htn::Planner::Status::PlanComplete);
		REQUIRE(testPlan.FinalCallList.size() == 2);
		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size() << "\n";
		PrintTaskCallList(testPlan.FinalCallList);
		std::cout << "\n\n";
	}

	SECTION("Failed decomposition of first goal method (one stack frame)")
	{
		std::cout << "TEST: Failed decomposition of first goal method (one stack frame)\n\n";
		// Goal task setup (first task fails)
		Htn::GoalTask failGoalTask;
		Htn::TaskList failMethods = {&failTaskTask, &goalTaskTask};
		// failMethods.push_back(&goalTaskTask);
		failGoalTask.SetMethods(&failMethods);
		Htn::Task failGoalTaskTask(&failGoalTask);
		Htn::TaskCall failGoalTaskCall = {&failGoalTaskTask, params};

		Htn::Planner testPlan;
		testPlan.InitialCallList.push_back(failGoalTaskCall);

		Htn::Planner::Status status;
		for (int i = 0; i < 12; i++)
		{
			status = testPlan.PlanStep();
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::Planner::Status::PlanComplete)
				break;
		}

		REQUIRE(status == Htn::Planner::Status::PlanComplete);
		REQUIRE(testPlan.FinalCallList.size() == 1);
		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size() << "\n";
		PrintTaskCallList(testPlan.FinalCallList);
		std::cout << "\n\n";
	}

	SECTION("Proper state change test (task with dependencies on another task being run before it)")
	{
		std::cout << "TEST: Proper state change test (task with dependencies on another task being "
		             "run before it)\n\n";
		Htn::Planner testPlan;
		testPlan.InitialCallList.push_back(nestedGoalTaskCall);
		testPlan.InitialCallList.push_back(requiresStateTaskCall);
		testPlan.State.TestStateChange = 0;

		Htn::Planner::Status status;
		for (int i = 0; i < 12; i++)
		{
			status = testPlan.PlanStep();
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::Planner::Status::PlanComplete)
				break;
		}

		REQUIRE(status == Htn::Planner::Status::PlanComplete);
		REQUIRE(testPlan.FinalCallList.size() == 2);
		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size() << "\n";
		PrintTaskCallList(testPlan.FinalCallList);
		std::cout << "\n\n";
	}

	SECTION(
	    "Proper state change test (task with dependencies on another task being run before it) "
	    "(This time, fail!)")
	{
		std::cout << "TEST: Proper state change test (task with dependencies on another task being "
		             "run before it) (This time, fail!)\n\n";
		Htn::Planner testPlan;
		testPlan.InitialCallList.push_back(requiresStateTaskCall);
		testPlan.InitialCallList.push_back(nestedGoalTaskCall);
		testPlan.State.TestStateChange = 0;

		Htn::Planner::Status status;
		for (int i = 0; i < 12; i++)
		{
			status = testPlan.PlanStep();
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::Planner::Status::PlanComplete ||
			    status == Htn::Planner::Status::Failed_NoPossiblePlan)
				break;
		}

		REQUIRE(status == Htn::Planner::Status::Failed_NoPossiblePlan);
		REQUIRE(testPlan.FinalCallList.size() == 0);
		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size() << "\n";
		PrintTaskCallList(testPlan.FinalCallList);
		std::cout << "\n\n";
	}

	SECTION("Test user-specified early break settings")
	{
		std::cout << "TEST: User-specified early break settings\n\n";
		Htn::Planner testPlan;
		testPlan.InitialCallList.push_back(nestedGoalTaskCall);
		testPlan.InitialCallList.push_back(requiresStateTaskCall);
		testPlan.State.TestStateChange = 0;
		testPlan.BreakOnStackAction = false;
		testPlan.BreakOnCompoundDecomposition = false;
		testPlan.BreakOnPrimitiveApply = false;
		testPlan.BreakOnFailedDecomposition = false;

		int numIterations = 0;

		Htn::Planner::Status status;
		for (int i = 0; i < 12; i++)
		{
			numIterations++;
			status = testPlan.PlanStep();
			std::cout << "[" << i << "] Returned Status " << (int)status << "\n";
			if (status == Htn::Planner::Status::PlanComplete ||
			    status == Htn::Planner::Status::Failed_NoPossiblePlan)
				break;
		}

		REQUIRE(status == Htn::Planner::Status::PlanComplete);
		REQUIRE(testPlan.FinalCallList.size() == 2);
		REQUIRE(numIterations == 1);
		std::cout << "\n\nFinal Plan length: " << testPlan.FinalCallList.size() << "\n";
		PrintTaskCallList(testPlan.FinalCallList);
		std::cout << "\n\n";
	}
}