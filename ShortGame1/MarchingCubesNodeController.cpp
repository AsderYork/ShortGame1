#include "stdafx.h"
#include "MarchingCubesNodeController.h"


namespace GEM
{
	bool MarchingCubeController::Initialize()
	{
		try
		{
			m_Window = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("MarchingCubesNodeController.layout");
			auto DefWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
			DefWindow->addChild(m_Window);
			m_turnedOn = true;
		}
		catch (std::exception &e)
		{
			LOGCATEGORY("FPS_Layout/Initialize").error("Failed to initialize:%s", e.what());
			return false;
		}

		return true;
	}

	void MarchingCubeController::turnOn()
	{
	}

	void MarchingCubeController::turnOff()
	{
	}

	void MarchingCubeController::PreFrame(float delta)
	{
	}
}
