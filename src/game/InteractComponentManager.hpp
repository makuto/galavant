#include "../entityComponentSystem/ComponentManager.hpp"
#include "../entityComponentSystem/ComponentTypes.hpp"
#include "../world/WorldResourceLocator.hpp"

namespace gv
{
struct Pickup
{
	Entity entity;
	WorldResourceType ResourceType;
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
	InteractComponentManager() = default;
	virtual ~InteractComponentManager() = default;

	void CreatePickups(int count, PickupRefList& newPickups);
};
}