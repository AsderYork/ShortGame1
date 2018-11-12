#include "GS_EntityController.h"

#include "Mixin_Movable.h"

namespace GEM::GameSim
{
	bool EntityController::RemoveEntity(ENTITY_ID_TYPE id)
	{
		auto& it = m_entityMap.find(id);
		if (it != m_entityMap.end())
		{
			m_entityMap.erase(it);
			return true;
		}
		return false;
	}
	std::weak_ptr<EntityBase> EntityController::GetEntity(ENTITY_ID_TYPE id)
	{
		auto& it = m_entityMap.find(id);
		if (it != m_entityMap.end())
		{ return it->second; }
		return std::weak_ptr<EntityBase>();
	}

	ENTITY_ID_TYPE EntityController::GetEntitiesCount() const
	{
		return static_cast<ENTITY_ID_TYPE>(m_entityMap.size());
	}


	std::list<std::weak_ptr<EntityBase>> EntityController::getAllEntitiesInASphere(const btVector3 center, const float Radius)
	{
		std::list<std::weak_ptr<EntityBase>> result;
		for (auto& ent : m_entityMap)
		{
			auto EntMovable = static_cast<Mixin_Movable*>(ent.second->GetMixinByID(Mixin_Movable::MixinID));
			if (EntMovable != nullptr  && EntMovable->getPos().distance(center) <= Radius)
			{
				result.push_back(std::weak_ptr(ent.second));
			}
		}
		return result;
	}
}