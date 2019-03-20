#pragma once
#include "GameSimService.h"
#include "ObjectVis_Simplistic_model.h"

#include <memory>
#include <map>

namespace GEM
{



	class ObjectVis_Simplistic {
	private:
		boost::signals2::connection m_createdConnection;
		boost::signals2::connection m_destroyedConnection;

		std::map<GameSim::ENTITY_ID_TYPE, GameModel> m_models;

	public:

		ObjectVis_Simplistic(GameSimController * gsController);
		~ObjectVis_Simplistic();

		void frame();

		void onEntitiyCreated(std::shared_ptr<GameSim::EntityBase> ent);
		void onEntitiyDestroyed(std::shared_ptr<GameSim::EntityBase> ent);
	
	
	};

}