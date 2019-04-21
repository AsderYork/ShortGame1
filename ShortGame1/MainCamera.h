#pragma once
#include "InputEvent.h"
#include <Mixin_Movable.h>
#include <EntitiesBase.h>

#include <OGRE/OgreCamera.h>


#include <memory>

namespace GEM
{
	/**!
	Camera for main character. With 3rd person and a 1st person option.
	*/
	class MainCamera
	{
	private:
		Ogre::SceneNode* m_cameraNode = nullptr;
		Ogre::Camera* m_camera = nullptr;
		std::weak_ptr<GameSim::EntityBase> m_entity;

		btVector3 m_cameraCenter;
		
		float m_azimuth = 0;
		float m_elevation = 0;
		float m_radius = 10;

		bool m_isActive = false;

	public:

		~MainCamera();

		bool getActive();
		void setActive(bool state);

		/**!
		Ties a camera to a given entity.
		Entity must exist and must be movable.
		*/
		void TieCamera(std::weak_ptr<GameSim::EntityBase> PlayerEntity);
		

		void UpdateCamera(float delta, const InputEvent_MouseState mouse, const std::vector<InputEvent_Button> buttons);

		/**
		Returns a direction, to which a player character must rotate to align with the camera.
		*/
		btQuaternion getOrientation();

		/**
		Returns a true orientation that is set to a scene node, that is a parent to a camera
		*/
		Ogre::Quaternion getNodeOrientation();

		Ogre::Vector3 getPos();

	};
}