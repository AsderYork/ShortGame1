#include "stdafx.h"
#include "GameVisualization.h"

#include <OGRE\OgreItem.h>

namespace GEM
{
	GameVisualization::GameVisualization(GameSimController * gs, Ogre_Service * ogre) : m_gsController(gs), m_ogreController(ogre),
		m_landscape(gs, ogre)
	{}
	Service::ActionResult GameVisualization::initialize()
	{
		return ActionResult::AR_OK;
	}
	void GameVisualization::shutdown()
	{
		m_landscape.Clear();
		m_visibleEntities.clear();
	}
	Service::ActionResult GameVisualization::preFrame(float timeDelta)
	{
		if (!m_gsController->getSimulationState()) { return ActionResult::AR_OK; }
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
					m_visibleEntities.emplace(MaybeEnityIter->first, ObjectRAII(m_ogreController));
					Visibility = m_visibleEntities.find(MaybeEnityIter->first);
				}
				Visibility->second.node->setPosition(Ogre::Vector3(Movability->getPos().x(), Movability->getPos().y(), Movability->getPos().z()));
				newMap.emplace(std::move(*Visibility));

			}

			MaybeEnityIter = m_gsController->m_entities.IterateOverEntities(std::move(iter));
		}
		
		m_visibleEntities = std::move(newMap);

		m_landscape.updateVisual();

		return ActionResult::AR_OK;
	}
	Service::ActionResult GameVisualization::frame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}

	Service::ActionResult GameVisualization::postFrame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}
	
	GameVisualization::ObjectRAII::ObjectRAII(ObjectRAII && other)
	{
		item = other.item;
		node = other.node;
		ogre = other.ogre;

		other.item = nullptr;
		other.node = nullptr;

	}

	GameVisualization::ObjectRAII::ObjectRAII(Ogre_Service * Ogre) : ogre(Ogre)
	{
		auto mSceneMgr = ogre->getRoot()->getSceneManager("ExampleSMInstance");
		item = mSceneMgr->createItem("Cubexa.mesh");

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(item);
	}

	GameVisualization::ObjectRAII::~ObjectRAII()
	{//So yeah, distructor is called even for moved-from objects. So we first have to check state and only then we can destroy
		if (node == nullptr) { return; }
		node->removeAndDestroyAllChildren();
		ogre->getRoot()->getSceneManager("ExampleSMInstance")->destroyItem(item);
	}

}