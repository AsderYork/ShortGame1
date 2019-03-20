#include "stdafx.h"

#include <OGRE/OgreRoot.h>
#include <OGRE\OgreItem.h>
#include <Ogre/Hlms/Pbs/OgreHlmsPbsDatablock.h>

#include "ObjectVis_Simplistic_model.h"


namespace GEM 
{
	GameModel::GameModel(std::shared_ptr<GameSim::EntityBase> ent) : m_entity(ent)
	{

		auto Movability = static_cast<GameSim::Mixin_Movable*>(ent->GetMixinByID(GameSim::Mixin_Movable::MixinID));

		auto mSceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("ExampleSMInstance");
		m_item = mSceneMgr->createItem("Cubexa.mesh");
		static int TmpNames = 0;
		m_item->getSubItem(0)->setDatablock(m_item->getSubItem(0)->getDatablock()->clone("PerPlayer" + std::to_string(TmpNames++)));

		m_node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		auto pos = Movability->getPos();
		m_PlayerNode = m_node->createChildSceneNode(Ogre::SCENE_DYNAMIC, Ogre::Vector3(pos.x(), pos.y(), pos.z()));
		m_PlayerNode->attachObject(m_item);

	}

	void GameModel::frame() {

		auto locked_ptr = m_entity.lock();

		if (locked_ptr) {
			auto Movability = static_cast<GameSim::Mixin_Movable*>(locked_ptr->GetMixinByID(GameSim::Mixin_Movable::MixinID));

			auto pos = Movability->getPos();
			m_node->setPosition(Ogre::Vector3(pos.x(), pos.y(), pos.z()));

			auto orient = Movability->getOrientation();
			m_node->setOrientation(orient.w(), orient.x(), orient.y(), orient.z());

			auto Health = static_cast<GameSim::Mixin_Health*>(locked_ptr->GetMixinByID(GameSim::Mixin_Health::MixinID));

			auto HealthPercent = Health->getHealth() / Health->getMaxHealth();

			auto FullHealthColor = Ogre::Vector3(0.2f, 1.0f, 0.2f);
			auto NoHealthColor = Ogre::Vector3(8.0f, 0.1f, 1.0f);
			auto ActualColor = FullHealthColor * HealthPercent + NoHealthColor * (1 - HealthPercent);

			static_cast<Ogre::HlmsPbsDatablock*>(m_item->getSubItem(0)->getDatablock())->setDiffuse(ActualColor);
		}

	}

	GameModel::~GameModel()
	{
		m_PlayerNode->removeAndDestroyAllChildren();
		m_node->removeAndDestroyAllChildren();
		Ogre::Root::getSingletonPtr()->getSceneManager("ExampleSMInstance")->destroyItem(m_item);
	}
}