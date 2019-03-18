#include "stdafx.h"
#include "ObjectVis_Simplistic.h"


namespace GEM {


	ObjectVis_Simplistic::ObjectVis_Simplistic(GameSimController * gsController) 
	{
		gsController->m_entities.registerCreateListener([this](auto& val) {onEntitiyCreated(val); });
		gsController->m_entities.registerDestroyListener([this](auto& val) {onEntitiyDestroyed(val); });

	}

	ObjectVis_Simplistic::~ObjectVis_Simplistic()
	{
		m_createdConnection.disconnect();
		m_destroyedConnection.disconnect();
	}


	void ObjectVis_Simplistic::onEntitiyCreated(std::shared_ptr<GameSim::EntityBase> ent) {
		m_models.emplace(ent->m_id, ent);
	}

	void ObjectVis_Simplistic::onEntitiyDestroyed(std::shared_ptr<GameSim::EntityBase> ent) {
		m_models.erase(ent->m_id);
	}

	GameModel::GameModel(std::shared_ptr<GameSim::EntityBase> ent)
	{
	}

}
