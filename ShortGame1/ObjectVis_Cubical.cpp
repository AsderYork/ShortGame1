#include "stdafx.h"
#include "ObjectVis_Cubical.h"

#include <OGRE/OgreRoot.h>
#include <OGRE\OgreItem.h>
#include <Ogre/Hlms/Pbs/OgreHlmsPbsDatablock.h>

namespace GEM
{
	ObjectVis_Cubical::ObjectVis_Cubical(GameSimController * gs) : m_gsController(gs)
	{}


	ObjectVis_Cubical::~ObjectVis_Cubical()
	{
		m_visibleEntities.clear();
	}

	void ObjectVis_Cubical::Frame()
	{
		std::map<GameSim::ENTITY_ID_TYPE, ObjectRAII> newMap;

		//Gather all the changes.
		std::unique_ptr<GameSim::EntityController::EntityListIterator> iter = nullptr;
		auto MaybeEnityIter = m_gsController->m_entities.IterateOverEntities(std::move(iter));

		while (MaybeEnityIter)
		{
			auto Movability = static_cast<GameSim::Mixin_Movable*>(MaybeEnityIter->second->GetMixinByID(GameSim::Mixin_Movable::MixinID));
			if (Movability != nullptr)
			{
				auto Visibility = m_visibleEntities.find(MaybeEnityIter->first);
				if (Visibility == m_visibleEntities.end())
				{
					m_visibleEntities.emplace(MaybeEnityIter->first, ObjectRAII());
					Visibility = m_visibleEntities.find(MaybeEnityIter->first);
				}
				Visibility->second.node->setPosition(Ogre::Vector3(Movability->getPos().x(), Movability->getPos().y(), Movability->getPos().z()));
				Visibility->second.node->setOrientation(Movability->getOrientation().w(), Movability->getOrientation().x(), Movability->getOrientation().y(), Movability->getOrientation().z());
				
				auto Health = static_cast<GameSim::Mixin_Health*>(MaybeEnityIter->second->GetMixinByID(GameSim::Mixin_Health::MixinID));

				auto HealthPercent = Health->getHealth() / Health->getMaxHealth();

				auto FullHealthColor = Ogre::Vector3(0.2f, 1.0f, 0.2f);
				auto NoHealthColor = Ogre::Vector3(1.0f, 0.1f, 0.1f);
				auto ActualColor = FullHealthColor * HealthPercent + NoHealthColor * (1 - HealthPercent);

				static_cast<Ogre::HlmsPbsDatablock*>(Visibility->second.item->getSubItem(0)->getDatablock())->setDiffuse(ActualColor);
				newMap.emplace(std::move(*Visibility));
			}

			MaybeEnityIter = m_gsController->m_entities.IterateOverEntities(std::move(iter));
		}

		m_visibleEntities = std::move(newMap);
	}
	
	
	ObjectVis_Cubical::ObjectRAII::ObjectRAII(ObjectRAII && other)
	{
		item = other.item;
		node = other.node;
		PlayerNode = other.PlayerNode;

		other.item = nullptr;
		other.node = nullptr;
		other.PlayerNode = nullptr;


	}

	ObjectVis_Cubical::ObjectRAII::ObjectRAII()
	{
		auto mSceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("ExampleSMInstance");
		item = mSceneMgr->createItem("Cubexa.mesh");
		static int TmpNames = 0;
		item->getSubItem(0)->setDatablock(item->getSubItem(0)->getDatablock()->clone("PerPlayer" + std::to_string(TmpNames++)));
		
		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		PlayerNode = node->createChildSceneNode(Ogre::SCENE_DYNAMIC, Ogre::Vector3(0.0f, 1.0f, 0.0f));
		PlayerNode->attachObject(item);
	}

	ObjectVis_Cubical::ObjectRAII::~ObjectRAII()
	{//So yeah, distructor is called even for moved-from objects. So we first have to check state and only then we can destroy
		if (node == nullptr) { return; }
		PlayerNode->removeAndDestroyAllChildren();
		node->removeAndDestroyAllChildren();
		Ogre::Root::getSingletonPtr()->getSceneManager("ExampleSMInstance")->destroyItem(item);
	}

}