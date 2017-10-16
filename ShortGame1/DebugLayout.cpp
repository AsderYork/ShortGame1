#include "stdafx.h"
#include "DebugLayout.h"
#include <OGRE\OgreCamera.h>

namespace GEM
{

	DebugLayout::DebugLayout(int SizeOfAvgGroup, Ogre_Service *OgreService) : m_sizeOfAvgGroup(SizeOfAvgGroup), m_ogreService(OgreService)
	{
		assert(SizeOfAvgGroup > 0);
	}
	bool DebugLayout::Initialize()
	{
		try
		{
			FPS_Window = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("FPS.layout");
			auto DefWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
			DefWindow->addChild(FPS_Window);
			m_turnedOn = true;
		}
		catch (std::exception &e)
		{
			LOGCATEGORY("FPS_Layout/Initialize").error("Failed to initialize:%s", e.what());
			return false;
		}

		return true;
	}

	void DebugLayout::turnOn()
	{
	}

	void DebugLayout::turnOff()
	{
	}

	void DebugLayout::PreFrame(float delta)
	{
		int tmpFPS = (int)(1 / delta);
		if (m_avgFPS == 0) { m_avgFPS = tmpFPS; }
		tmpFPS += m_avgFPS;
		m_avgFPS = tmpFPS / 2;

		FPS_Window->setText("FPS" + std::to_string(m_avgFPS) +
			"\nX:" + std::to_string((float)m_ogreService->getCamera()->getPosition().x) +
			"\nY:" + std::to_string((float)m_ogreService->getCamera()->getPosition().y) +
			"\nZ:" + std::to_string((float)m_ogreService->getCamera()->getPosition().z) );
	}


}
