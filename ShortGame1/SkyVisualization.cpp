#include "stdafx.h"
#include "SkyVisualization.h"
#include <LogHelper.h>
#include <OGRE\OgreRoot.h>
#include <LinearMath/btVector3.h>

#define TICK_TIME 20

namespace GEM
{
	void SkyVisualization::Shutdown()
	{
	}
	void SkyVisualization::StartVisualization()
	{
		auto root = Ogre::Root::getSingletonPtr();
		m_caelumSystem = std::make_unique<Caelum::CaelumSystem>(root, root->getSceneManager("ExampleSMInstance"),
			(Caelum::CaelumSystem::CaelumComponent)
			(
				Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME
				| Caelum::CaelumSystem::CAELUM_COMPONENT_SUN
				| Caelum::CaelumSystem::CAELUM_COMPONENT_IMAGE_STARFIELD
				| Caelum::CaelumSystem::CAELUM_COMPONENT_MOON
				)
			);
		m_caelumSystem->setTimeScale(m_timeOfDayCoeff);
		
	}

	void SkyVisualization::frame()
	{
		if (!m_oneFrameSkipped) { m_oneFrameSkipped = true; return; }
		auto cam = Ogre::Root::getSingletonPtr()->getSceneManager("ExampleSMInstance")->getCameras().front();
		auto TimeDelta = GameSim::GameTimeToSeconds(m_gs->getGameTime() - m_lastGameTime);
		m_lastGameTime = m_gs->getGameTime();

		m_caelumSystem->updateSubcomponents(TimeDelta);
		m_caelumSystem->notifyCameraChanged(cam);
	}
}
