#include "stdafx.h"
#include "DebugMapGraphicsService.h"
#include <OGRE\OgreItem.h>
#include <OGRE\OgreCamera.h>

#include <OGRE\OgreHlms.h>
#include <OGRE\OgreHlmsDatablock.h>
#include <OGRE\Hlms\Pbs\OgreHlmsPbsDatablock.h>
#include <OGRE\OgreHlmsManager.h>

namespace GEM
{
	void DebugMapGraphicsService::CreateShader()
	{
		Ogre::HlmsManager *hlmsManager = m_ogreService->getRoot()->getHlmsManager();
		Ogre::HlmsTextureManager *hlmsTextureManager = hlmsManager->getTextureManager();

		auto hlmsPbs = hlmsManager->getHlms(Ogre::HLMS_PBS);

		Ogre::String datablockName = "DMGS_VisionMat";

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

		datablock->setDiffuse(Ogre::Vector3(0.05f, 0.4f, 0.1f));
		datablock->setSpecular(Ogre::Vector3(0.0f, 0.0f, 0.0f));
		datablock->setRoughness(1.0f);
		datablock->setBrdf(Ogre::PbsBrdf::BlinnPhongFullLegacy);
		datablock->setTransparency(0.15f);

		Ogre::HlmsMacroblock macro = *(datablock->getMacroblock());
		macro.mCullMode = Ogre::CULL_NONE;
		datablock->setMacroblock(macro);
	}
	Service::ActionResult DebugMapGraphicsService::initialize()
	{
		auto mSceneMgr = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");

		auto ogreEntity1 = mSceneMgr->createItem("Cubexa.mesh");
		CreateShader();
		ogreEntity1->setDatablock("DMGS_VisionMat");

		m_visionNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		
		//m_visionNode->scale(Ogre::Vector3(1, 1, 1));
		m_visionNode->attachObject(ogreEntity1);

		return ActionResult();
	}

	void DebugMapGraphicsService::shutdown()
	{
	}

	Service::ActionResult DebugMapGraphicsService::preFrame(float timeDelta)
	{
		auto Pos = m_ogreService->getCamera()->getPosition() + m_ogreService->getCamera()->getDerivedDirection() * 10;

		switch (m_action)
		{
#pragma warning( push )
#pragma warning( disable : 4244)//Round doesn't cast float to int. But it just int
		case ACTION_FILL:
		{
			m_mapService->SetIndividualNode(round(Pos.x), round(Pos.y), round(Pos.z), 255);
			break;
		}
		case ACTION_DIG:
		{
			m_mapService->SetIndividualNode(round(Pos.x), round(Pos.y), round(Pos.z), 0);
			break;
		}
#pragma warning( pop ) 
		default: {break;}
		}

		m_visionNode->setPosition(Pos);
		return ActionResult();
	}

	Service::ActionResult DebugMapGraphicsService::frame(float timeDelta)
	{
		return ActionResult();
	}

	Service::ActionResult DebugMapGraphicsService::postFrame(float timeDelta)
	{
		return ActionResult();
	}

	void DebugMapGraphicsService::mouseMoved(const SDL_Event & arg)
	{
		

	}

	void DebugMapGraphicsService::mousePressed(const SDL_MouseButtonEvent & arg)
	{
		if (arg.button == SDL_BUTTON_LEFT)
		{
			m_action = ACTION_FILL;
		}
		else if (arg.button == SDL_BUTTON_RIGHT)
		{
			m_action = ACTION_DIG;
		}
	}

	void DebugMapGraphicsService::mouseReleased(const SDL_MouseButtonEvent & arg)
	{

		m_action = ACTION_NONE;
	}

}








//OgreMeshTool -v2 "C:\Users\Asder\Documents\Visual Studio 2017\Projects\ShortGame1\build\Resources\media\models\cube.mesh" "C:\Users\Asder\Documents\Visual Studio 2017\Projects\ShortGame1\build\Resources\media\Unique\cubeOri.mesh"