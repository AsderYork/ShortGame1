#pragma once
#include "InputEvent.h"

#include <OGRE/OgreRay.h>
#include <OGRE/OgreSceneQuery.h>
#include <OGRE/OgreCamera.h>
#include <vector>
#include <memory>

namespace GEM
{

	class DebugVisualization
	{
	private:
		bool m_isInited = false;
		Ogre::Camera* m_camera;
		Ogre::SceneManager* m_sceneManager;
		Ogre::Ray m_ray;
		Ogre::RaySceneQuery* m_query;
	
	public:

		void init();

		void frame(float delta, const InputEvent_MouseState mouse, const std::vector<InputEvent_Button> buttonEvents);

	};

}