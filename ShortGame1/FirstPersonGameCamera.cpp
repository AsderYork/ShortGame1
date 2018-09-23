#include "stdafx.h"
#include "FirstPersonGameCamera.h"
#include <GameSimulation.h>
#include <LogHelper.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneNode.h>
#include <chrono>

namespace GEM
{

	void FirstPersonGameCamera::UpdateCamera()
	{
		if (m_entity.expired()) { return; }
		auto LockedEnt = m_entity.lock();
		auto Movable = static_cast<GameSim::Mixin_Movable*>(LockedEnt->GetMixinByID(GameSim::Mixin_Movable::MixinID));

		static auto znow = std::chrono::steady_clock::now();
		float ChanVal = std::chrono::duration<float>(std::chrono::steady_clock::now() - znow).count();
		//Movable->SetOrientation(btQuaternion(btVector3(0.0f, 1.0f, 0.0f), ChanVal / 10));

		auto Pos = Movable->getPos();
		auto Rot = Movable->getOrientation();

		//Rot = btQuaternion(0.0f, 0.0f, 0.0f);
		btVector3 Shift(0, 4, -10);

	

		Shift.rotate(Rot.getAxis(), Rot.getAngle());
		Pos += quatRotate(Rot, Shift);
		Rot *= btQuaternion(SIMD_PI, -SIMD_PI*0.04f, 0.0f);
		//Rot *= btQuaternion(std::chrono::duration<float>(std::chrono::steady_clock::now()- znow).count()*0.3, 0.0f, 0);

		//Shift Camera sligtly, so that object would remain visible
		m_cameraNode->setPosition(Pos.x(), Pos.y(), Pos.z());
		m_cameraNode->setOrientation(Ogre::Quaternion(Rot.getW(), Rot.getX(), Rot.getY(), Rot.getZ()));
	}

	void FirstPersonGameCamera::TieCamera(std::weak_ptr<GameSim::EntityBase> PlayerEntity)
	{
		m_entity = PlayerEntity;
		if (m_entity.expired())
		{
			LOGCATEGORY("Camera\TieCamera").error("Can't tie camera to provided entity. There is no such entity!");
			return;
		}

		auto Movable = static_cast<GameSim::Mixin_Movable*>(m_entity.lock()->GetMixinByID(GameSim::Mixin_Movable::MixinID));
		if (Movable == nullptr)
		{
			LOGCATEGORY("Camera\TieCamera").error("Can't tie camera to provided entity. This entity doesn't have a movable mixin!");
			return;
		}

		m_cameraNode = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->getRootSceneNode()->createChildSceneNode();
		m_cameraNode->setName("PlayerCameraNode");
		Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->getCameras()[0]->detachFromParent();

		m_cameraNode->attachObject(	Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->getCameras()[0]);
	}
}