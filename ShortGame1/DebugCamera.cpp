#include "stdafx.h"
#include "DebugCamera.h"

#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneNode.h>
#include <SDL2/SDL_keyboard.h>

namespace GEM
{

	void DebugCamera::init()
	{
		m_camera = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->getCameras()[0];

		m_cameraNode = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->getRootSceneNode()->createChildSceneNode();
		m_cameraNode->setName("NewDebugCamera");
		m_cameraNode->setPosition(0.0f, 0.0f, 0.0f);
	}

	DebugCamera::~DebugCamera()
	{
		if (m_cameraNode)
		{
			m_cameraNode->removeAndDestroyAllChildren();
			Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->destroySceneNode(m_cameraNode);
		}
	}

	void DebugCamera::setActive(bool active)
	{
		if (m_isActive != active && active) {

				m_camera->detachFromParent();
				m_cameraNode->attachObject(m_camera);	

		}

		m_isActive = active;
	}

	bool DebugCamera::isActive()
	{
		return m_isActive;
	}

	void DebugCamera::frame(float delta, const InputEvent_MouseState mouse, const std::vector<InputEvent_Button> buttons)
	{
		if (m_cameraNode == nullptr || !m_isActive) {return;}

		auto OgreRot = m_cameraNode->getOrientation();

		auto rot = btQuaternion(0.0f, 0.0f, 0.0f);
		auto Shift = btVector3(0, 0, 0);


		if (mouse.middleButton || mouse.rightButton)
		{
			m_yawRot *= btQuaternion(-mouse.rel_x*0.006f, 0.0f, 0.0f);
			m_pitchRot *= btQuaternion(0.0f, -mouse.rel_y*0.006, 0.0f);
			

			auto keyboardState = SDL_GetKeyboardState(nullptr);

			Shift = btVector3(0.2*(1.0f*keyboardState[SDL_SCANCODE_RIGHT] - 1.0f*keyboardState[SDL_SCANCODE_LEFT]),
								0.2*(1.0f*keyboardState[SDL_SCANCODE_SPACE] - 1.0f*keyboardState[SDL_SCANCODE_LCTRL]),
								0.2*(1.0f*keyboardState[SDL_SCANCODE_DOWN] - 1.0f*keyboardState[SDL_SCANCODE_UP]));
		}


		rot = m_yawRot * m_pitchRot;
		m_cameraNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
		m_cameraNode->translate(Ogre::Vector3(Shift.x(), Shift.y(), Shift.z()), Ogre::Node::TS_LOCAL);

	}

	void DebugCamera::setPosition(Ogre::Vector3 pos)
	{
		if (m_isActive) {m_cameraNode->setPosition(pos.x, pos.y, pos.z);}
	}

	void DebugCamera::setOrientation(Ogre::Quaternion quat)
	{
		if (m_isActive)
		{
			float theta = atan2(quat.y, quat.w);
			m_pitchRot = btQuaternion(0.0f, sin(theta), 0.0f, cos(theta));
		}
	}

}