#pragma once
#include "InputEvent.h"

#include <OGRE/OgreCamera.h>
#include <LinearMath/btQuaternion.h>


namespace GEM {

	/**
	Provides a way to view the game free of any gameplay constraints.
	*/
	class DebugCamera {

	private:
		Ogre::Camera* m_camera = nullptr;
		Ogre::SceneNode* m_cameraNode = nullptr;

		btQuaternion m_yawRot = btQuaternion(0.0f, 0.0f, 0.0f);
		btQuaternion m_pitchRot = btQuaternion(0.0f, 0.0f, 0.0f);


		bool m_isActive = false;

	public:

		~DebugCamera();

		void init();

		/**
		Set true to activate this camera, false to turn it off. Only one camera should be on at any given time
		*/
		void setActive(bool active);
		
		void setPosition(Ogre::Vector3 pos);
		void setOrientation(Ogre::Quaternion);

		/**
		Returns true if this camera is active. False otherwise
		*/
		bool isActive();

		void frame(float delta, const InputEvent_MouseState mouse, const std::vector<InputEvent_Button> buttons);

	};

}