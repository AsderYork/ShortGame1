#include "stdafx.h"
#include "ObjectVis_Simplistic.h"



namespace GEM {


	ObjectVis_Simplistic::ObjectVis_Simplistic(GameSimController * gsController) 
	{
		m_createdConnection = gsController->m_entities.registerCreateListener([this](auto val) {onEntitiyCreated(val); });
		m_destroyedConnection = gsController->m_entities.registerDestroyListener([this](auto val) {onEntitiyDestroyed(val); });

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

	void ObjectVis_Simplistic::frame(float delta) {

		for (auto& model : m_models) {
			model.second.frame(delta);
		}

	}

}
