#include "stdafx.h"
#include "MarchingToOgre.h"
#include <OGRE\OgreItem.h>
#include <OGRE\OgreMaterialManager.h>
#include <OGRE\OgreMaterial.h>

#include <OGRE\OgreMeshManager2.h>
#include <OGRE\OgreMesh2.h>
#include <OGRE\OgreSubMesh2.h>
#include <OGRE\OgreSubItem.h>
#include <OGRE\OgreEntity.h>

#include <OGRE\OgreHlms.h>
#include <OGRE\OgreHlmsDatablock.h>
#include <OGRE\Hlms\Pbs\OgreHlmsPbsDatablock.h>
#include <OGRE\OgreHlmsManager.h>

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
				//Makes different material for different meshes
				std::string matName("MarchingMat" + std::to_string(m_itemsVector.size()));
				//auto MatForObject = DefaultMat->clone(matName);
				//auto Colortoset = Ogre::ColourValue(SceneMap[i][j][k] / 256.0, SceneMap[i][j][k] / 256.0, SceneMap[i][j][k] / 256.0, 1);

				auto Colortoset = Ogre::ColourValue(1, 1, 0, 1);
				//MatForObject->setAmbient(Colortoset);
				//tmpItem->setMaterial(MatForObject);

				LOGCATEGORY("MATINUSE").info("%s", tmpItem->getMesh()->getSubMesh(0)->getMaterialName().c_str());
				//auto Colortoset = Ogre::ColourValue(1, 1, 0, 1);
				CreateOrUpdateDatablock(id, SceneMap[i][j][k] / 256.0);
				tmpItem->setDatablock("Test" + std::to_string(id));
				

				tmpItem->getMesh()->getSubMesh(0)->getMaterialName().c_str();

				tmpSceneNode->attachObject(tmpItem);
				tmpSceneNode->setScale(Ogre::Vector3(m_meshScale, m_meshScale, m_meshScale));
				tmpSceneNode->setPosition(i*m_mapScale, j*m_mapScale, k*m_mapScale);

				m_itemsVector.push_back(tmpItem);
				m_nodesVector.push_back(tmpSceneNode);
				id++;
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
	}

	datablock->setDiffuse(Ogre::Vector3(0.0f, value, 0.0f));
	datablock->setTransparency(0.8);

}

void GEM::MarchingToOgre::mouseMoved(const SDL_Event & arg)
{
}

void GEM::MarchingToOgre::mousePressed(const SDL_MouseButtonEvent & arg)
{
	if (arg.button == SDL_BUTTON_LEFT)
	{

	}
}

void GEM::MarchingToOgre::mouseReleased(const SDL_MouseButtonEvent & arg)
{
}
