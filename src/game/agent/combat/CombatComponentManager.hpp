#pragma once

#include "entityComponentSystem/ComponentManager.hpp"
#include "util/ResourceDictionary.hpp"

namespace gv
{
typedef struct CombatFx CombatFx;

struct CombatActionDef
{
	enum class CombatActionType
	{
		None = 0,

		Attack,

		CombatActionType_count
	};
	CombatActionType Type;

	float Duration;

	// TODO: These aren't relevant to all types of actions
	struct CombatActionDamage
	{
		ResourceKey AffectsNeed;
		float Duration;
		float Amount;
	};

	CombatActionDamage Damage;

	struct CombatActionKnockback
	{
		float Duration;
		float Distance;
	};

	CombatActionKnockback Knockback;
};

extern ResourceDictionary<CombatActionDef> g_CombatActionDefDictionary;

// Contains an instance of a combat action, i.e. the specific conditions unique to this action
struct CombatAction
{
	CombatActionDef* Def;
	CombatFx* Fx;

	// Users need not set this
	float ActivateTime;
};

struct Combatant
{
	Entity entity;

	enum class CombatState
	{
		None = 0,

		Attacking,
		Defending,
		Staggered,

		CombatState_count
	};
	CombatState State;

	CombatAction CurrentAction;
};
typedef std::vector<Combatant> CombatantList;
typedef std::vector<Combatant*> CombatantRefList;

class CombatFxHandler
{
public:
	virtual ~CombatFxHandler() = default;

	virtual void OnActivateCombatAction(Entity combatant, CombatAction& action) = 0;
};

class CombatComponentManager : public ComponentManager
{
public:
	CombatFxHandler* FxHandler;

private:
	CombatantList Combatants;

	float TimeToNextUpdate;

protected:
	virtual void UnsubscribeEntitiesInternal(const EntityList& entities);

public:
	CombatComponentManager();
	virtual ~CombatComponentManager() = default;

	void Initialize(CombatFxHandler* fxHandler);

	void CreateCombatants(const EntityList& entities, CombatantRefList& newCombatants);

	Combatant* GetCombatant(Entity combatantEntity);

	void ActivateCombatAction(Entity combatant, CombatAction action);

	void Update(float deltaSeconds);
};

extern CombatComponentManager g_CombatComponentManager;
}