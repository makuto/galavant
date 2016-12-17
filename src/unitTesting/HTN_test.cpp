#include <iostream>

#include "../ai/htn/HTNTypes.hpp"
#include "../ai/htn/HTNTasks.hpp"

class TestCompoundTaskA : public Htn::CompoundTask
{
public:
	TestCompoundTaskA(void)
	{
		//Type = Htn::TaskType::Primitive;
	}

	virtual ~TestCompoundTaskA(void)
	{
	}

	virtual bool StateMeetsPreconditions(const Htn::TaskArguments& args)
	{
		return true;
	}

	virtual bool Decompose(Htn::TaskList& taskList, const Htn::TaskArguments& args)
	{
		std::cout << "Decompose TestCompoundTaskA: " << args.Parameters[0].IntValue << "\n";
		return true;
	}
};

class TestPrimitiveTask : public Htn::PrimitiveTask
{
public:
	TestPrimitiveTask(void)
	{
		//Type = Htn::TaskType::Primitive;
	}

	virtual ~TestPrimitiveTask(void)
	{
	}

	virtual bool StateMeetsPreconditions(const Htn::TaskArguments& args)
	{
		return true;
	}

	virtual bool Execute(const Htn::TaskArguments& args)
	{
		std::cout << "execute TestPrimitiveTask: " << args.Parameters[0].IntValue << "\n";
		return true;
	}
};

int main()
{
	// Test parameters
	Htn::Parameter testParam = {Htn::Parameter::ParamType::Int, 123};
	std::cout << testParam.FloatValue << "\n";

	// Test Task types
	TestPrimitiveTask testPrimitive;
	std::cout << (int)testPrimitive.GetType() << "\n";

	// Goal task setup
	TestCompoundTaskA testCompoundTaskAA;
	TestCompoundTaskA testCompoundTaskAB;
	const Htn::TaskList goalTaskMethods = {&testCompoundTaskAA, &testCompoundTaskAB};

	return 0;
}