#pragma once
#include "Ogre_Service.h"
#include "GameSimService.h"

#include <memory>

namespace GEM {
	/**
	Encapsulates lifecycle of a game model
	*/
	class GameModel {

	private:
		Ogre::Item* m_item;
		Ogre::SceneNode* m_node;
		Ogre::SceneNode* m_PlayerNode;

		std::weak_ptr<GameSim::EntityBase> m_entity;

	public:
		GameModel(std::shared_ptr<GameSim::EntityBase> ent);
		virtual ~GameModel();

		void frame();

		GameModel(const GameModel&) = delete;
		GameModel& operator= (const GameModel&) = delete;
	};
}