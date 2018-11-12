#include "Object_Player.h"

namespace GEM::GameSim
{
	std::optional<bool> Object_Player_DeathBlast::CheckValidity(const Dependecies & Deps)
	{
		//Deadly blast is allways possible. It's just too good!
		return true;
	}

	void Object_Player_DeathBlast::ApplyEvent(Dependecies & Deps)
	{
		auto thisMovable = static_cast<Mixin_Movable*>(static_cast<Object_Player*>(Deps.thisObject)->GetMixinByID(Mixin_Movable::MixinID));
		auto AffectedItems = Deps.entityController->getAllEntitiesInASphere(thisMovable->getPos(), 10);
		for(auto& object : AffectedItems)
		{
			auto LockedObject = object.lock();
			auto TheirHealth = static_cast<Mixin_Health*>(static_cast<EntityBase*>(&*LockedObject)->GetMixinByID(Mixin_Health::MixinID));
			if (TheirHealth != nullptr)
			{
				if ((LockedObject->m_id != Deps.thisObject->m_id) && TheirHealth->m_health > 0)
				{
					TheirHealth->m_health -= 10;
				}
			}
		}
	}

	void Object_Player_DeathBlast::RollbackEvent(Dependecies & Deps)
	{
		//You can't rollback the DeathBlast. It's just too good!
	}

	std::optional<bool> Object_Player_Heal::CheckValidity(const Dependecies & Deps)
	{
		//Deadly blast is allways possible. It's just too good!
		return true;
	}

	void Object_Player_Heal::ApplyEvent(Dependecies & Deps)
	{
		auto thisMovable = static_cast<Mixin_Movable*>(static_cast<Object_Player*>(Deps.thisObject)->GetMixinByID(Mixin_Movable::MixinID));
		auto AffectedItems = Deps.entityController->getAllEntitiesInASphere(thisMovable->getPos(), 10);
		for (auto& object : AffectedItems)
		{
			auto LockedObject = object.lock();
			auto TheirHealth = static_cast<Mixin_Health*>(static_cast<EntityBase*>(&*LockedObject)->GetMixinByID(Mixin_Health::MixinID));
			if (TheirHealth != nullptr)
			{
				if ((LockedObject->m_id != Deps.thisObject->m_id) && TheirHealth->m_health > 0)
				{
					TheirHealth->m_health -= 10;
				}
			}
		}
	}

	void Object_Player_Heal::RollbackEvent(Dependecies & Deps)
	{
		//You can't rollback the DeathBlast. It's just too good!
	}
}
