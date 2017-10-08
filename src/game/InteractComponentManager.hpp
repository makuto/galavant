#pragma once

#include "entityComponentSystem/ComponentManager.hpp"
#include "world/WorldResourceLocator.hpp"
#include "agent/NeedTypes.hpp"

namespace gv
{
struct Pickup
{
	Entity entity;
	NeedType AffectsNeed;

	// Destroy the entire entity when picked up
	bool DestroySelfOnPickup;
};
typedef std::vector<Pickup> PickupList;
typedef std::vector<Pickup*> PickupRefList;

class InteractComponentManager : public ComponentManager
{
private:
	PickupList Pickups;

protected:
	virtual void UnsubscribeEntitiesInternal(const EntityList& entities);

public:
	InteractComponentManager();
	virtual ~InteractComponentManager() = default;

	void CreatePickups(const EntityList& entities, PickupRefList& newPickups);

	bool PickupDirect(Entity pickupEntity, Entity claimer);

	Pickup* GetPickup(Entity pickupEntity);
};

extern InteractComponentManager g_InteractComponentManager;
}