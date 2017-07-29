#include "stdafx.h"
#include "MarchingToOgre.h"
#include <OGRE\OgreItem.h>

GEM::MarchingToOgre::MarchingToOgre(std::string MeshName, Ogre_Service* OgreService, MarchingCubesCalculator * Calc, int MapScale, int MeshScale)
	:m_meshName(MeshName), m_ogreService(OgreService), m_calc(Calc), m_mapScale(MapScale), m_meshScale(MeshScale)
{

}

GEM::Service::ActionResult GEM::MarchingToOgre::initialize()
{
	auto SceneMap = m_calc->getMap();
	auto SceneManager = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");
	for (int i = 0; i < SceneMap.size(); i++)
	{
		for (int j = 0; j < SceneMap[i].size(); j++)
		{
			for (int k = 0; k < SceneMap[i][j].size(); k++)
			{
				Ogre::Item *tmpItem = SceneManager->createItem(m_meshName);
				Ogre::SceneNode *tmpSceneNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
				tmpSceneNode->attachObject(tmpItem);
				tmpSceneNode->setScale(Ogre::Vector3(m_meshScale, m_meshScale, m_meshScale));
				tmpSceneNode->setPosition(i*m_mapScale, j*m_mapScale, k*m_mapScale);
				m_itemsVector.push_back(tmpItem);
				m_nodesVector.push_back(tmpSceneNode);
			}
		}
	}
	
	return ActionResult();
}

void GEM::MarchingToOgre::shutdown()
{
	for (auto& item : m_itemsVector)
	{
		item->detachFromParent();
	}
}

GEM::Service::ActionResult GEM::MarchingToOgre::preFrame(double timeDelta)
{
	return ActionResult();
}

GEM::Service::ActionResult GEM::MarchingToOgre::frame(double timeDelta)
{
	return ActionResult();
}

GEM::Service::ActionResult GEM::MarchingToOgre::postFrame(double timeDelta)
{
	return ActionResult();
}
