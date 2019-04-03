#pragma once
#include "InputEvent.h"
#include "GameSimulation.h"

#include <OGRE/OgreSceneQuery.h>
#include <OGRE/OgreCamera.h>
#include <vector>
#include <memory>

namespace GEM
{

	class DebugVisualization
	{
	private:
		GameSim::GameSimulation* m_gsController;

		bool m_isInited = false;
		Ogre::Camera* m_camera;
		Ogre::SceneManager* m_sceneManager;
		Ogre::RaySceneQuery* m_query;

		float m_pickingRange = 1000.0f;

		Ogre::Item* item;
		Ogre::SceneNode* node;
	
	public:

		DebugVisualization(GameSim::GameSimulation* gsController);

		void init();

		void frame(float delta, const InputEvent_MouseState mouse, const std::vector<InputEvent_Button> buttonEvents);

	};

}