#pragma once
#include <OGRE/OgreLight.h>


namespace GEM
{
	/**!
	Visualizes sky. Sun, moon, stars e.t.c. Maybe later there will be clouds and other weather effects
	*/
	class SkyVisualization
	{
	private:
		bool m_isActive = false;

		Ogre::Light* m_light;
		Ogre::SceneNode* m_lightNode;

		void Shutdown();
		

	public:
		/**!
		SkyVisualization depends on a WorldTime object which might not be avaliable at the beginning.
		But it should be created after the first package from the server! This method must be as soon as
		WorldTime object is created.
		*/
		void StartVisualization();

		void frame();
	};
}