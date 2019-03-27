#include "stdafx.h"

#include <OGRE/OgreRoot.h>
#include <OGRE\OgreItem.h>
#include <Ogre/Hlms/Pbs/OgreHlmsPbsDatablock.h>

#include "ObjectVis_Simplistic_model.h"

static Ogre::SkeletonAnimation* anim;

class VariableWithDamping {

public:
	float m_currValue;
	float m_dampingCoef;
	float m_targetValue;

	void reset(float currVal, float dampingCoef)
	{
		m_currValue = currVal;
		m_dampingCoef = dampingCoef;
		m_targetValue = currVal;

	}

	void frame(float delta)
	{
		if (m_dampingCoef*delta > 1) 
		{
			m_currValue = m_targetValue;
		}
		else
		{
			m_currValue += ((m_targetValue - m_currValue)*m_dampingCoef)*delta;
		}
	}

};


static VariableWithDamping damping;



namespace GEM 
{
	GameModel::GameModel(std::shared_ptr<GameSim::EntityBase> ent) : m_entity(ent)
	{

		auto Movability = static_cast<GameSim::Mixin_Movable*>(ent->GetMixinByID(GameSim::Mixin_Movable::MixinID));

		auto mSceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("ExampleSMInstance");
		m_item = mSceneMgr->createItem("dummy.mesh");

		m_skeleton = m_item->getSkeletonInstance();
		//m_animations.emplace("Idle", m_skeleton->getAnimation("Idle"));
		//m_animations.emplace("Walk", m_skeleton->getAnimation("Walk"));

		//anim = m_skeleton->getAnimation("Walk");

		for (auto& item : m_skeleton->getAnimations()) {
			LOGCATEGORY("GameModel/GameModel").info("Anim:" + item.getName().getFriendlyText());
			m_animations.emplace(item.getName().getFriendlyText(), m_skeleton->getAnimation(item.getName().getFriendlyText()));
		}

		m_animations["Idle"]->setLoop(true);
		m_animations["Idle"]->setEnabled(true);
		m_animations["Walk"]->setLoop(true);
		m_animations["Walk"]->setEnabled(true);


		damping.reset(0.0f, 10.0f);

		//anim->setLoop(true);
		//anim->setEnabled(true);
		//anim->mWeight = 1.0f;



		static int TmpNames = 0;
		m_item->getSubItem(0)->setDatablock(m_item->getSubItem(0)->getDatablock()->clone("PerPlayer" + std::to_string(TmpNames++)));

		m_node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		auto pos = Movability->getPos();
		m_PlayerNode = m_node->createChildSceneNode(Ogre::SCENE_DYNAMIC, Ogre::Vector3(pos.x(), pos.y(), pos.z()));
		m_PlayerNode->attachObject(m_item);

	}

	void GameModel::frame(float delta) {

		auto locked_ptr = m_entity.lock();

		if (locked_ptr) {
			auto Movability = static_cast<GameSim::Mixin_Movable*>(locked_ptr->GetMixinByID(GameSim::Mixin_Movable::MixinID));

			auto pos = Movability->getPos();
			m_node->setPosition(Ogre::Vector3(pos.x(), pos.y(), pos.z()));

			auto spd = Movability->getVelocity().length();
			auto max_spd = Movability->getSpeed();

			auto spd_fract = spd / max_spd;
			damping.m_targetValue = spd_fract;
			damping.frame(delta);

			m_animations["Walk"]->mWeight = damping.m_currValue;
			m_animations["Idle"]->mWeight = 1 - damping.m_currValue;



			auto orient = Movability->getOrientation();
			m_node->setOrientation(orient.w(), orient.x(), orient.y(), orient.z());

			auto Health = static_cast<GameSim::Mixin_Health*>(locked_ptr->GetMixinByID(GameSim::Mixin_Health::MixinID));

			auto HealthPercent = Health->getHealth() / Health->getMaxHealth();

			auto FullHealthColor = Ogre::Vector3(0.2f, 1.0f, 0.2f);
			auto NoHealthColor = Ogre::Vector3(8.0f, 0.1f, 1.0f);
			auto ActualColor = FullHealthColor * HealthPercent + NoHealthColor * (1 - HealthPercent);

			static_cast<Ogre::HlmsPbsDatablock*>(m_item->getSubItem(0)->getDatablock())->setDiffuse(ActualColor);
		}

		//anim->addTime(delta);
		for (auto& item : m_animations) {
			item.second->addTime(delta);
		}

	}

	GameModel::~GameModel()
	{
		m_PlayerNode->removeAndDestroyAllChildren();
		m_node->removeAndDestroyAllChildren();
		Ogre::Root::getSingletonPtr()->getSceneManager("ExampleSMInstance")->destroyItem(m_item);
	}
}