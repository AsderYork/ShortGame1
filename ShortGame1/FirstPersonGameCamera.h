#pragma once
#include <Mixin_Movable.h>

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
		Ogre::Camera* m_camera;
	public:

		/**!
		Ties a camera to a given entity.
		Entity must exist and must be movable.
		*/
		void TieCamera(GameSim::ENTITY_ID_TYPE entID, GameSim::GameSimulation* gs);
	};
}