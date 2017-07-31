#include "stdafx.h"
#include "MarchingToOgre.h"
#include <OGRE\OgreItem.h>
#include <OGRE\OgreMaterialManager.h>
#include <OGRE\OgreMaterial.h>

#include <cmath>


#include <OGRE\OgreHlms.h>
#include <OGRE\OgreHlmsDatablock.h>
#include <OGRE\Hlms\Pbs\OgreHlmsPbsDatablock.h>
#include <OGRE\OgreHlmsManager.h>

#include <OGRE\OgreCamera.h>
#include <OGRE\OgreSceneQuery.h>


GEM::MarchingToOgre::MarchingToOgre(std::string MeshName, Ogre_Service* OgreService, MarchingCubesCalculator * Calc, int MapScale, int MeshScale)
	:m_meshName(MeshName), m_ogreService(OgreService), m_calc(Calc), m_mapScale(MapScale), m_meshScale(MeshScale)
{

}

void GEM::MarchingToOgre::updatePrepresentation()
{
	auto SceneMap = m_calc->getMap();
	int ObjNum = 0;
	for (int i = 0; i < SceneMap.size(); i++)
	{
		for (int j = 0; j < SceneMap[i].size(); j++)
		{
			for (int k = 0; k < SceneMap[i][j].size(); k++)
			{				
				ObjNum++;
			}
		}
	}
}

GEM::Service::ActionResult GEM::MarchingToOgre::initialize()
{
	auto SceneMap = m_calc->getMap();
	auto SceneManager = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");

	
	int id = 0;
	for (int i = 0; i < SceneMap.size(); i++)
	{
		for (int j = 0; j < SceneMap[i].size(); j++)
		{
			for (int k = 0; k < SceneMap[i][j].size(); k++)
			{
				Ogre::Item *tmpItem = SceneManager->createItem(m_meshName);
				Ogre::SceneNode *tmpSceneNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);			
				CreateOrUpdateDatablock(id, SceneMap[i][j][k] / 256.0);
				tmpItem->setDatablock("Test" + std::to_string(id));	

				tmpSceneNode->attachObject(tmpItem);
				tmpSceneNode->setScale(Ogre::Vector3(m_meshScale, m_meshScale, m_meshScale));
				tmpSceneNode->setPosition(i*m_mapScale, j*m_mapScale, k*m_mapScale);

				m_itemsVector.push_back(tmpItem);
				m_nodesVector.push_back(tmpSceneNode);
				id++;
			}
		}
	}

	CreateSelectionDatablock();
	m_SelectionCountur = SceneManager->createItem(m_meshName);
	m_SelectionCountur->setDatablock("Picking");

	m_SelectionCounturNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
	m_SelectionCounturNode->setScale(Ogre::Vector3(m_meshScale*1.08, m_meshScale*1.08, m_meshScale*1.08));
	m_SelectionCounturNode->setPosition(0, 0,0);
	m_SelectionCounturNode->attachObject(m_SelectionCountur);
	m_SelectionCounturNode->setVisible(false);
	
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
	int id = 0;
	auto SceneMap = m_calc->getMap();


	for (int i = 0; i < SceneMap.size(); i++)
	{
		for (int j = 0; j < SceneMap[i].size(); j++)
		{
			for (int k = 0; k < SceneMap[i][j].size(); k++)
			{
				CreateOrUpdateDatablock(id, SceneMap[i][j][k] / 256.0);
				id++;
			}
		}
	}
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

void GEM::MarchingToOgre::CreateOrUpdateDatablock(int id, float value)
{
	Ogre::HlmsManager *hlmsManager = m_ogreService->getRoot()->getHlmsManager();
	Ogre::HlmsTextureManager *hlmsTextureManager = hlmsManager->getTextureManager();

	auto hlmsPbs = hlmsManager->getHlms(Ogre::HLMS_PBS);

	Ogre::String datablockName = "Test" + Ogre::StringConverter::toString(id);

	Ogre::HlmsPbsDatablock *datablock;
	datablock = static_cast<Ogre::HlmsPbsDatablock*>(hlmsPbs->getDatablock(datablockName));
	if (datablock == nullptr)
	{
		datablock = static_cast<Ogre::HlmsPbsDatablock*>(
			hlmsPbs->createDatablock(datablockName,
				datablockName,
				Ogre::HlmsMacroblock(),
				Ogre::HlmsBlendblock(),
				Ogre::HlmsParamVec()));

		datablock->setDiffuse(Ogre::Vector3(0.7f, 0.7f, 0.1f));
		Ogre::HlmsMacroblock macro = *(datablock->getMacroblock());
		macro.mCullMode = Ogre::CULL_NONE;
		datablock->setMacroblock(macro);

		datablock->setTransparency(0.8);

	}

	datablock->setDiffuse(Ogre::Vector3(0.0f, value, 0.0f));

}

void GEM::MarchingToOgre::CreateSelectionDatablock()
{
	Ogre::HlmsManager *hlmsManager = m_ogreService->getRoot()->getHlmsManager();
	Ogre::HlmsTextureManager *hlmsTextureManager = hlmsManager->getTextureManager();

	auto hlmsPbs = hlmsManager->getHlms(Ogre::HLMS_PBS);

	Ogre::String datablockName = "Picking";

	Ogre::HlmsPbsDatablock *datablock;
	datablock = static_cast<Ogre::HlmsPbsDatablock*>(hlmsPbs->getDatablock(datablockName));
	if (datablock == nullptr)
	{
		datablock = static_cast<Ogre::HlmsPbsDatablock*>(
			hlmsPbs->createDatablock(datablockName,
				datablockName,
				Ogre::HlmsMacroblock(),
				Ogre::HlmsBlendblock(),
				Ogre::HlmsParamVec()));
	}

	datablock->setDiffuse(Ogre::Vector3(1.0f, 1.0f, 1.0f));
	datablock->setRoughness(0.0f);
	datablock->setBrdf(Ogre::PbsBrdf::BlinnPhongFullLegacy);

	Ogre::HlmsMacroblock macro = *(datablock->getMacroblock());
	macro.mCullMode = Ogre::CULL_ANTICLOCKWISE;
	datablock->setMacroblock(macro);

}

void GEM::MarchingToOgre::mouseMoved(const SDL_Event & arg)
{
}

void GEM::MarchingToOgre::mousePressed(const SDL_MouseButtonEvent & arg)
{
	if (arg.button == SDL_BUTTON_LEFT)
	{
	 auto ray =	m_ogreService->getCamera()->getCameraToViewportRay(0.5, 0.5);
	 Ogre::RaySceneQuery *Querry = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance")->createRayQuery(ray);
	 
	 auto Result = Querry->execute();
	 if (Result.size() > 0)
	 {
		 m_SelectionCounturNode->setPosition( Result.front().movable->getParentNode()->getPosition());
		 m_SelectionCounturNode->setVisible(true);
	 }
	 else
	 {
		 m_SelectionCounturNode->setVisible(false);
	 }
	}

	if (arg.button == SDL_BUTTON_RIGHT)
	{
		if (m_SelectionCountur->isVisible())//If something is chosen
		{
			int x, y, z;//Position of node in a map
			
			auto SceneNodePose = m_SelectionCounturNode->getPosition();
			x = std::round(SceneNodePose.x / m_mapScale);
			y = std::round(SceneNodePose.y / m_mapScale);
			z = std::round(SceneNodePose.z / m_mapScale);

			if (m_calc->getValueOfNode(x, y, z) != 0)
			{
				m_calc->setValueOfNode(x, y, z, 0);
			}
			else
			{
				m_calc->setValueOfNode(x, y, z, 255);
			}
		}
	}
}

void GEM::MarchingToOgre::mouseReleased(const SDL_MouseButtonEvent & arg)
{
}
