#include "stdafx.h"
#include "DebugVisualization.h"


#include <OGRE/OgreRay.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreRoot.h>
#include <OGRE\OgreItem.h>
#include <Ogre/Hlms/Pbs/OgreHlmsPbsDatablock.h>

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

namespace GEM
{

	DebugVisualization::DebugVisualization(GameSim::GameSimulation* gsController)
	{
		m_gsController = gsController;
	}

	void DebugVisualization::init()
	{
		m_isInited = true;

		m_sceneManager = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");

		m_camera = m_sceneManager->getCameras()[0];

		auto mSceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("ExampleSMInstance");
		item = mSceneMgr->createItem("Cubexa.mesh");
		static int TmpNames = 0;

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::SCENE_DYNAMIC, Ogre::Vector3(0.0f, 1.0f, 0.0f));
		node->setScale(0.3f, 0.3f, 0.3f);


	}

	void DebugVisualization::frame(float delta, const InputEvent_MouseState mouse, const std::vector<InputEvent_Button> buttonEvents)
	{
		if (!m_isInited) {
			return;
		}

		bool SelectButton = mouse.leftButton;
			

		if (SelectButton)
		{
			auto ScreenRay = m_camera->getCameraToViewportRay(mouse.screenSpaceX, mouse.screenSpaceY);
			auto RayOrigin = ScreenRay.getOrigin();
			auto OgreEnd = ScreenRay.getPoint(m_pickingRange);

			auto from = btVector3(RayOrigin.x, RayOrigin.y, RayOrigin.z);
			auto to = btVector3(OgreEnd.x, OgreEnd.y, OgreEnd.z);

			btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
			closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

			m_gsController->m_physics.getWorld()->rayTest(from, to, closestResults);

			if (closestResults.hasHit())
			{
				btVector3 p = from.lerp(to, closestResults.m_closestHitFraction);
				node->setPosition(Ogre::Vector3(p.x(), p.y(), p.z()));
				if (!item->isAttached()) { node->attachObject(item);}
			}
			else {
				node->detachAllObjects();
			}

		}
		

	}

}