#include "stdafx.h"
#include "DebugLayout.h"
#include <OGRE\OgreCamera.h>

namespace GEM
{

	DebugLayout::DebugLayout(int SizeOfAvgGroup, Ogre_Service *OgreService, MapService* mapService) :
		m_ogreService(OgreService),
		m_mapService(mapService)
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

		m_fps_ForThisSecond += delta;
		m_frames++;

		if (m_fps_ForThisSecond > 1.0f)
		{
			m_fps_lastSecond = m_frames / m_fps_ForThisSecond;
			m_fps_ForThisSecond = 0.0f;
			m_frames = 0;
		}

		auto CamPos = m_ogreService->getCamera()->getPosition();

		std::string Text = "FPS" + std::to_string(m_fps_lastSecond) +
			"\nX:" + std::to_string((float)CamPos.x) +
			"\nY:" + std::to_string((float)CamPos.y) +
			"\nZ:" + std::to_string((float)CamPos.z) + "\n";

		if (m_mapService != nullptr)
		{
			auto Res = m_mapService->getChunk(CamPos.x, CamPos.y, CamPos.z);
			Text += "Chunk< " + std::to_string(Res.first) + " : " + std::to_string(Res.second) + " >\n";
		}


		FPS_Window->setText(Text);
	}


}
