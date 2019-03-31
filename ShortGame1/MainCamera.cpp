#include "stdafx.h"
#include "MainCamera.h"
#include <GameSimulation.h>
#include <LogHelper.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneNode.h>

#include <algorithm>

namespace GEM 
{
	void MainCamera::UpdateCamera(float delta, const InputEvent_MouseState mouse, const std::vector<InputEvent_Button> buttons)
	{
		if (m_entity.expired() || !m_isActive) { return; }
		auto LockedEnt = m_entity.lock();
		auto Movable = static_cast<GameSim::Mixin_Movable*>(LockedEnt->GetMixinByID(GameSim::Mixin_Movable::MixinID));

		static auto znow = std::chrono::steady_clock::now();

		auto Pos = Movable->getPos();
		auto Rot = Movable->getOrientation();
		

		if (mouse.middleButton || mouse.rightButton)
		{
			m_elevation += mouse.rel_y*0.006;
			m_azimuth -= mouse.rel_x*0.006;

		}
		m_radius -= mouse.rel_wheel / 45.0f;
		m_radius = std::clamp(m_radius, 0.0f, 1.0f);


		Rot.setEuler(0, m_elevation, 0);
		Rot = btQuaternion(m_azimuth, 0, 0) * Rot;

		btVector3 Shift(0, 0, -12* m_radius);
		btVector3 Height(0, 1.8 + 2 * m_radius, 0);



		Shift.rotate(Rot.getAxis(), Rot.getAngle());
		Pos += quatRotate(Rot, Shift);
		Pos += Height;
		Rot *= btQuaternion(SIMD_PI, -SIMD_PI * 0.04f, 0.0f);

		//Shift Camera sligtly, so that object would remain visible
		m_cameraNode->setPosition(Pos.x(), Pos.y(), Pos.z());
		m_cameraNode->setOrientation(Ogre::Quaternion(Rot.getW(), Rot.getX(), Rot.getY(), Rot.getZ()));
	}

	void MainCamera::TieCamera(std::weak_ptr<GameSim::EntityBase> PlayerEntity)
	{
		m_azimuth = 0;
		m_elevation = 0;
		m_radius = 0.8;

		m_entity = PlayerEntity;
		if (m_entity.expired())
		{
			LOGCATEGORY("Camera/TieCamera").error("Can't tie camera to provided entity. There is no such entity!");
			return;
		}

		auto Movable = static_cast<GameSim::Mixin_Movable*>(m_entity.lock()->GetMixinByID(GameSim::Mixin_Movable::MixinID));
		if (Movable == nullptr)
		{
			LOGCATEGORY("Camera/TieCamera").error("Can't tie camera to provided entity. This entity doesn't have a movable mixin!");
			return;
		}

		m_camera = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->getCameras()[0];

		m_cameraNode = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->getRootSceneNode()->createChildSceneNode();
		m_cameraNode->setName("PlayerCameraNode");
	}

	bool MainCamera::getActive()
	{
		return m_isActive;
	}

	void MainCamera::setActive(bool state) 
	{
		if (m_isActive != state && state) {
			m_camera->detachFromParent();
			m_cameraNode->attachObject(m_camera);		

		}

		m_isActive = state;
	}

	MainCamera::~MainCamera() 
	{
		if (m_cameraNode)
		{
			m_cameraNode->removeAndDestroyAllChildren();
			Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->destroySceneNode(m_cameraNode);
		}
	}


	btQuaternion MainCamera::getOrientation()
	{
		btQuaternion Rot(0, m_elevation, 0);
		Rot = btQuaternion(m_azimuth, 0, 0) * Rot;
		return Rot;
	}


	Ogre::Quaternion MainCamera::getNodeOrientation() {
		return m_cameraNode->getOrientation();
	}

	Ogre::Vector3 MainCamera::getPos()
	{
		return m_cameraNode->getPosition();
	}
}