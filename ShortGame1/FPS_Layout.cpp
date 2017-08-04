#include "stdafx.h"
#include "FPS_Layout.h"
#include <CEGUI\CEGUI.h>

namespace GEM
{
	FPS_Layout::FPS_Layout(int SizeOfAvgGroup) : m_sizeOfAvgGroup(SizeOfAvgGroup)
	{		
		assert(SizeOfAvgGroup > 0);
	}
	bool FPS_Layout::Initialize()
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

	void FPS_Layout::turnOn()
	{
	}

	void FPS_Layout::turnOff()
	{
	}

	void FPS_Layout::PreFrame(float delta)
	{
		int tmpFPS = (int)(1 / delta);
		if (m_avgFPS == 0) { m_avgFPS = tmpFPS; }
		tmpFPS += m_avgFPS;
		m_avgFPS = tmpFPS / 2;

		FPS_Window->setText("FPS" + std::to_string(m_avgFPS));
	}

}
