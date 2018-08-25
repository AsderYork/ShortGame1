#include "GS_EntityController.h"

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

}