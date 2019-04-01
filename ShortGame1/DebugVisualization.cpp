#include "stdafx.h"
#include "DebugVisualization.h"


#include <OGRE/OgreCamera.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneNode.h>

namespace GEM
{

	void DebugVisualization::init() {
		m_isInited = true;

		m_sceneManager = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");

		m_camera = m_sceneManager->getCameras()[0];
		m_query = m_sceneManager->createRayQuery(m_ray);

	}

	void DebugVisualization::frame(float delta, const InputEvent_MouseState mouse, const std::vector<InputEvent_Button> buttonEvents)
	{
		if (!m_isInited) {
			return;
		}

		bool SelectButton = false;
		for (auto& button : buttonEvents)
		{
			if (button.code == SDL_SCANCODE_LCTRL && mouse.leftButton && button.is_pressed)
			{
				SelectButton = true;
			}
		}

		if (SelectButton)
		{
			auto RealDirection = m_camera->getRealDirection();
			auto RealPosition = m_camera->getRealPosition();

			m_ray.setDirection(RealDirection);
			m_ray.setOrigin(RealPosition);

			auto result = m_query->execute();

			if (result.size() > 0)
			{
				for (auto& item : result)
				{
					item.distance;
				}
			}

		}
		

	}

}