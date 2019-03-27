#pragma once
#include "Ogre_Service.h"
#include "GameSimService.h"
#include <Ogre/Animation/OgreSkeletonAnimation.h>
#include <Ogre/Animation/OgreSkeletonInstance.h>

#include <map>
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
		std::map<std::string, Ogre::SkeletonAnimation*> m_animations;
		Ogre::SkeletonInstance* m_skeleton;

		std::weak_ptr<GameSim::EntityBase> m_entity;

	public:
		GameModel(std::shared_ptr<GameSim::EntityBase> ent);
		virtual ~GameModel();

		void frame(float delta);

		GameModel(const GameModel&) = delete;
		GameModel& operator= (const GameModel&) = delete;
	};
}