#include "util/Logging.hpp"

#include "world/WorldResourceLocator.hpp"
#include "world/ProceduralWorld.hpp"
#include "entityComponentSystem/EntityTypes.hpp"
#include "entityComponentSystem/EntityComponentManager.hpp"
#include "game/agent/PlanComponentManager.hpp"
#include "game/agent/AgentComponentManager.hpp"
#include "game/agent/combat/CombatComponentManager.hpp"
#include "game/InteractComponentManager.hpp"
#include "game/agent/Needs.hpp"
#include "ai/htn/HTNTasks.hpp"
#include "game/agent/htnTasks/MovementTasks.hpp"
#include "game/agent/htnTasks/InteractTasks.hpp"
#include "util/CallbackContainer.hpp"
#include "game/EntityLevelOfDetail.hpp"
#include "util/StringHashing.hpp"

#include "ConsoleMovementComponentManager.hpp"

#include <iostream>

#include <cstring>
#include <cstdlib>
#include <cstdio>

// @Callback: CustomLogOutputFunc
void ConsoleOnlyLogOutput(const gv::Logging::Record& record)
{
	bool IsWarning = (record.severity == gv::Logging::Severity::warning);
	bool IsError = (record.severity <= gv::Logging::Severity::error);

	static char funcNameBuffer[256];
	gv::Logging::FormatFuncName(funcNameBuffer, record.Function, sizeof(funcNameBuffer));

	static char buffer[2048];
	snprintf(buffer, sizeof(buffer), "%s():%lu: %s", funcNameBuffer, (unsigned long)record.Line,
	         record.OutBuffer);

	if (IsError)
	{
		std::cout << "Error: " << buffer << "\n";
	}
	else if (IsWarning)
	{
		std::cout << "Warning: " << buffer << "\n";
	}
	else
	{
		std::cout << "Log: " << buffer << "\n";
	}
}

static gv::Logging::Logger s_UnrealLogger(gv::Logging::Severity::debug, &ConsoleOnlyLogOutput);

void InitializeResources()
{
	// Hunger Need
	{
		static gv::NeedDef TestHungerNeed;
		TestHungerNeed.Type = gv::NeedType::Hunger;
		TestHungerNeed.Name = "Hunger";
		TestHungerNeed.InitialLevel = 70.f;
		TestHungerNeed.MaxLevel = 300.f;
		TestHungerNeed.MinLevel = 0.f;
		TestHungerNeed.UpdateRate = 10.f;
		TestHungerNeed.AddPerUpdate = 10.f;

		// Find food goal def
		{
			Htn::Parameter resourceToFind;
			resourceToFind.IntValue = gv::WorldResourceType::Food;
			resourceToFind.Type = Htn::Parameter::ParamType::Int;
			Htn::ParameterList parameters = {resourceToFind};
			Htn::TaskCall getResourceCall{Htn::g_TaskDictionary.GetResource(RESKEY("GetResource")),
			                              parameters};
			Htn::TaskCall pickupResourceCall{
			    Htn::g_TaskDictionary.GetResource(RESKEY("InteractPickup")), parameters};
			Htn::TaskCallList getResourceTasks = {getResourceCall, pickupResourceCall};
			static gv::AgentGoalDef s_findFoodGoalDef;
			s_findFoodGoalDef.Type = gv::AgentGoalDef::GoalType::HtnPlan;
			s_findFoodGoalDef.NumRetriesIfFailed = 2;
			s_findFoodGoalDef.Tasks = getResourceTasks;
			gv::g_AgentGoalDefDictionary.AddResource(RESKEY("FindFood"), &s_findFoodGoalDef);
		}

		// Hunger Need Triggers
		{
			gv::NeedLevelTrigger lookForFood;
			lookForFood.Condition = gv::NeedLevelTrigger::ConditionType::GreaterThanLevel;
			lookForFood.Level = 100.f;
			lookForFood.NeedsResource = true;
			lookForFood.WorldResource = gv::WorldResourceType::Food;
			TestHungerNeed.LevelTriggers.push_back(lookForFood);

			gv::NeedLevelTrigger desperateLookForFood;
			desperateLookForFood.Condition = gv::NeedLevelTrigger::ConditionType::GreaterThanLevel;
			desperateLookForFood.Level = 200.f;
			desperateLookForFood.GoalDef =
			    gv::g_AgentGoalDefDictionary.GetResource(RESKEY("FindFood"));
			TestHungerNeed.LevelTriggers.push_back(desperateLookForFood);

			gv::NeedLevelTrigger deathByStarvation;
			deathByStarvation.Condition = gv::NeedLevelTrigger::ConditionType::GreaterThanLevel;
			deathByStarvation.Level = 290.f;
			deathByStarvation.SetConsciousState = gv::AgentConsciousState::Dead;
			TestHungerNeed.LevelTriggers.push_back(deathByStarvation);
		}

		gv::g_NeedDefDictionary.AddResource(RESKEY("Hunger"), &TestHungerNeed);
	}

	// Blood Need
	{
		static gv::NeedDef BloodNeed;
		BloodNeed.Type = gv::NeedType::Blood;
		BloodNeed.Name = "Blood";
		BloodNeed.InitialLevel = 100.f;
		BloodNeed.MaxLevel = 100.f;
		BloodNeed.MinLevel = 0.f;
		// Agents will only gain blood over time, but there's a maximum and they start at max
		BloodNeed.UpdateRate = 10.f;
		BloodNeed.AddPerUpdate = 10.f;

		// Blood Need Triggers
		{
			gv::NeedLevelTrigger lowBloodUnconscious;
			lowBloodUnconscious.Condition = gv::NeedLevelTrigger::ConditionType::LessThanLevel;
			lowBloodUnconscious.Level = 20.f;
			lowBloodUnconscious.SetConsciousState = gv::AgentConsciousState::Unconscious;
			BloodNeed.LevelTriggers.push_back(lowBloodUnconscious);

			gv::NeedLevelTrigger deathByBleedingOut;
			deathByBleedingOut.Condition = gv::NeedLevelTrigger::ConditionType::Zero;
			deathByBleedingOut.SetConsciousState = gv::AgentConsciousState::Dead;
			BloodNeed.LevelTriggers.push_back(deathByBleedingOut);
		}

		gv::g_NeedDefDictionary.AddResource(RESKEY("Blood"), &BloodNeed);
	}

	// Goals
	{
		static gv::AgentGoalDef s_getResourceGoalDef;
		s_getResourceGoalDef.Type = gv::AgentGoalDef::GoalType::GetResource;
		s_getResourceGoalDef.NumRetriesIfFailed = 2;
		gv::g_AgentGoalDefDictionary.AddResource(RESKEY("GetResource"), &s_getResourceGoalDef);
	}

	// CombatActionDefs
	{
		// Punch
		{
			static gv::CombatActionDef s_punchAction;
			s_punchAction.Type = gv::CombatActionDef::CombatActionType::Attack;
			s_punchAction.Duration = .75f;
			s_punchAction.Damage = {RESKEY("Blood"), 10.f, 50.f};
			s_punchAction.Knockback = {1.f, 500.f};

			gv::g_CombatActionDefDictionary.AddResource(RESKEY("Punch"), &s_punchAction);
		}
	}
}

void InitializeEntityTests()
{
	// Create a couple test entities
	int numTestEntities = 20;
	gv::EntityList testEntities;
	testEntities.reserve(numTestEntities);
	gv::g_EntityComponentManager.GetNewEntities(testEntities, numTestEntities);

	// Add Movement components to all of them
	/*{
	    UnrealMovementComponent::UnrealMovementComponentList newEntityMovementComponents(
	        numTestEntities);

	    float spacing = 500.f;
	    int i = 0;
	    for (gv::EntityListIterator it = testEntities.begin(); it != testEntities.end(); ++it, i++)
	    {
	        newEntityMovementComponents[i].entity = (*it);
	        newEntityMovementComponents[i].data.ResourceType = gv::WorldResourceType::Agent;
	        newEntityMovementComponents[i].data.SpawnParams.CharacterToSpawn =
	            DefaultAgentCharacter;
	        newEntityMovementComponents[i].data.WorldPosition.Set(0.f, i * spacing,
	                                                              TestEntityCreationZ);
	        newEntityMovementComponents[i].data.GoalManDistanceTolerance = 600.f;
	        newEntityMovementComponents[i].data.MaxSpeed = 500.f;
	    }

	    g_ConsoleMovementComponentManager.SubscribeEntities(newEntityMovementComponents);
	}*/

	// Setup agent components for all of them and give them a need
	{
		gv::Need hungerNeed(RESKEY("Hunger"));
		gv::Need bloodNeed(RESKEY("Blood"));

		// TODO: Will eventually need a thing which creates agents based on a creation def and sets
		// up the needs accordingly

		gv::AgentComponentManager::AgentComponentList newAgentComponents(numTestEntities);

		int i = 0;
		for (gv::PooledComponent<gv::AgentComponentData>& currentAgentComponent :
		     newAgentComponents)
		{
			currentAgentComponent.entity = testEntities[i++];
			currentAgentComponent.data.Needs.push_back(hungerNeed);
			currentAgentComponent.data.Needs.push_back(bloodNeed);
		}

		gv::g_AgentComponentManager.SubscribeEntities(newAgentComponents);
	}

	// Add food
	{
		int numFood = 4;

		gv::EntityList testFoodEntities;
		testFoodEntities.reserve(numFood);
		gv::g_EntityComponentManager.GetNewEntities(testFoodEntities, numFood);

		// UnrealMovementComponent::UnrealMovementComponentList newFood(numFood);
		gv::PickupRefList newPickups;
		newPickups.reserve(numFood);
		gv::g_InteractComponentManager.CreatePickups(testFoodEntities, newPickups);

		//float spacing = 2000.f;
		int i = 0;
		for (gv::EntityListIterator it = testFoodEntities.begin(); it != testFoodEntities.end();
		     ++it, i++)
		{
			// Movement component
			/*{
			    FVector location(-2000.f, i * spacing, TestEntityCreationZ);

			    newFood[i].entity = (*it);
			    newFood[i].data.WorldPosition.Set(location.X, location.Y, location.Z);
			    newFood[i].data.ResourceType = gv::WorldResourceType::Food;

			    newFood[i].data.SpawnParams.ActorToSpawn = TestFoodActor;
			}*/

			// Pickup component
			if (i < (int)newPickups.size())
			{
				newPickups[i]->AffectsNeed = gv::NeedType::Hunger;
				newPickups[i]->DestroySelfOnPickup = true;
			}
		}

		// g_ConsoleMovementComponentManager.SubscribeEntities(newFood);
	}
}

void InitializeProceduralWorld()
{
	gv::ProceduralWorld::ProceduralWorldParams& Params =
	    gv::ProceduralWorld::GetActiveWorldParams();

	Params.WorldCellMaxHeight = 200.f;
	Params.WorldCellMinHeight = -10000.f;

	Params.Seed = 5138008;

	for (int i = 0; i < 3; i++)
		Params.WorldCellTileSize[i] = 120.f;

	// Hardcoded noise values because I won't be changing these often
	Params.ScaledNoiseParams.lowBound = 0.f;
	Params.ScaledNoiseParams.highBound = 255.f;
	Params.ScaledNoiseParams.octaves = 10;
	Params.ScaledNoiseParams.scale = 0.00001f;
	Params.ScaledNoiseParams.persistence = 0.55f;
	Params.ScaledNoiseParams.lacunarity = 2.f;
}

void InitializeGalavant()
{
	LOGI << "Initializing Galavant...";

	InitializeProceduralWorld();

	gv::WorldResourceLocator::ClearResources();

	// Initialize Entity Components
	{
		// I originally made this happen via static initialization, but using that in combination
		// with A) split Galavant static library and GalavantUnreal library and B) Unreal
		// Hotreloading caused issues (mainly that Unreal-specific ComponentManagers weren't being
		// registered in the same list)
		gv::g_EntityComponentManager.AddComponentManager(&gv::g_InteractComponentManager);
		gv::g_EntityComponentManager.AddComponentManager(&gv::g_CombatComponentManager);
		gv::g_EntityComponentManager.AddComponentManager(&gv::g_AgentComponentManager);
		gv::g_EntityComponentManager.AddComponentManager(&gv::g_PlanComponentManager);
		gv::g_EntityComponentManager.AddComponentManager(&g_ConsoleMovementComponentManager);

		static gv::CallbackContainer<Htn::TaskEventCallback> TaskEventCallbacks;

		g_ConsoleMovementComponentManager.Initialize(&TaskEventCallbacks);

		{
			static gv::WorldStateManager WorldStateManager;
			gv::g_PlanComponentManager.Initialize(&WorldStateManager, &TaskEventCallbacks);
			gv::g_PlanComponentManager.DebugPrint = true;
		}

		{
			gv::g_AgentComponentManager.Initialize(&gv::g_PlanComponentManager);
			gv::g_AgentComponentManager.DebugPrint = true;
		}

		//gv::g_CombatComponentManager.Initialize(&CombatFxHandler);
		gv::g_CombatComponentManager.Initialize(nullptr);
	}

	// Initialize Tasks
	{
		static gv::FindResourceTask FindResourceTask;
		static gv::MoveToTask MoveToTask;
		static gv::GetResourceTask GetResourceTask;
		static gv::InteractPickupTask InteractPickupTask;

		MoveToTask.Initialize(&g_ConsoleMovementComponentManager);
		GetResourceTask.Initialize(&FindResourceTask, &MoveToTask);
		InteractPickupTask.Initialize(&gv::g_InteractComponentManager);

		Htn::g_TaskDictionary.AddResource(RESKEY("FindResource"), FindResourceTask.GetTask());
		Htn::g_TaskDictionary.AddResource(RESKEY("MoveTo"), MoveToTask.GetTask());
		Htn::g_TaskDictionary.AddResource(RESKEY("GetResource"), GetResourceTask.GetTask());
		Htn::g_TaskDictionary.AddResource(RESKEY("InteractPickup"), InteractPickupTask.GetTask());
	}

	InitializeResources();

	// Initialize test levels
	{
		InitializeEntityTests();
	}

	// Initialize LOD settings
	{
		float PlayerManhattanViewDistance = 10000.f;
		gv::EntityLOD::g_EntityLODSettings.PlayerManhattanViewDistance =
		    PlayerManhattanViewDistance;
	}

	LOGI << "Galavant Initialized";
}

static float s_currentWorldTime = 0.f;

int main()
{
	InitializeGalavant();

	LOGI << "******* Starting main loop ********";

	// Main loop
	float totalRuntime = 100.f;
	float tickRate = 1.f;
	int numTicks = 0;
	for (float currentTime = 0.f; currentTime < totalRuntime; currentTime += tickRate)
	{
		s_currentWorldTime = currentTime;
		float deltaTime = tickRate;

		gv::g_EntityComponentManager.DestroyEntitiesPendingDestruction();

		//GalavantMain.Update(deltaTime);

		gv::g_CombatComponentManager.Update(deltaTime);
		gv::g_AgentComponentManager.Update(deltaTime);
		gv::g_PlanComponentManager.Update(deltaTime);
		g_ConsoleMovementComponentManager.Update(deltaTime);

		numTicks++;
	}

	LOGI << "******** Finished; cleaning up... (ticked "<< numTicks << " times) ********";

	// Cleanup
	{
		gv::g_EntityComponentManager.DestroyAllEntities();
		LOGI << "Destroyed all entities";
		gv::WorldResourceLocator::ClearResources();
		gv::ResourceDictionaryBase::ClearAllDictionaries();
	}

	LOGI << "Exiting";

	return 1;
}

// Latelinked functions
namespace gv
{
float GetWorldTime()
{
	return s_currentWorldTime;
}
}