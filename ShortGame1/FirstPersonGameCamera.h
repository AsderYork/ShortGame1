#pragma once
#include <Mixin_Movable.h>
#include <EntitiesBase.h>

#include <OGRE/OgreCamera.h>

#include <memory>

namespace GEM
{
	/**!
	This class provides basic means of tying camera to a \c movable game object.
	*/
	class FirstPersonGameCamera
	{
	private:
		Ogre::SceneNode* m_cameraNode;
		Ogre::Camera* m_camera;
		std::weak_ptr<GameSim::EntityBase> m_entity;


	public:

		/**!
		Ties a camera to a given entity.
		Entity must exist and must be movable.
		*/
		void TieCamera(std::weak_ptr<GameSim::EntityBase> PlayerEntity);


		void UpdateCamera();
	};
}