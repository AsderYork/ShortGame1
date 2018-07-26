#include "stdafx.h"
#include "SkyVisualization.h"
#include <LogHelper.h>
#include <OGRE\OgreRoot.h>

#define TICK_TIME 20

namespace GEM
{
	void SkyVisualization::Shutdown()
	{
	}
	void SkyVisualization::StartVisualization()
	{
		auto SceneManager = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");

		m_light = SceneManager->createLight();
		m_lightNode = SceneManager->getRootSceneNode()->createChildSceneNode();

		m_isActive = true;
		m_lightNode->attachObject(m_light);
		m_light->setPowerScale(Ogre::Math::PI); //Since we don't do HDR, counter the PBS' division by PI
		m_light->setType(Ogre::Light::LT_DIRECTIONAL);
		m_light->setDirection(Ogre::Vector3(-1, -1, -1).normalisedCopy());
	}

	void SkyVisualization::frame()
	{
		if (m_isActive)
		{

		btVector3 NightColor(105/255, 48/255, 142/255);
		btVector3 DayColor(252/255, 241/255, 179/255);

		auto DiffVal = (DayColor - NightColor)/ TICK_TIME;

		auto TimeInSeconds = 0.0f;

			

		auto ActualColor = NightColor + DiffVal * (TimeInSeconds - (((int)round(TimeInSeconds))% TICK_TIME) );


		m_light->setDiffuseColour(ActualColor.x, ActualColor.y, ActualColor.z);

		}
	}
}
