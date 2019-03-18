#pragma once
#include "GameSimService.h"

#include <memory>
#include <map>

namespace GEM
{
	/**
	Encapsulates lifecycle of a game model
	*/
	class GameModel {
	public:
		GameModel(std::shared_ptr<GameSim::EntityBase> ent);
		virtual ~GameModel();

		GameModel(const GameModel&) = delete;
		GameModel& operator= (const GameModel&) = delete;		
	};


	class ObjectVis_Simplistic {
	private:
		boost::signals2::connection m_createdConnection;
		boost::signals2::connection m_destroyedConnection;

		std::map<GameSim::ENTITY_ID_TYPE, GameModel> m_models;

	public:

		ObjectVis_Simplistic(GameSimController * gsController);
		~ObjectVis_Simplistic();

		void onEntitiyCreated(std::shared_ptr<GameSim::EntityBase> ent);
		void onEntitiyDestroyed(std::shared_ptr<GameSim::EntityBase> ent);
	
	
	};

}