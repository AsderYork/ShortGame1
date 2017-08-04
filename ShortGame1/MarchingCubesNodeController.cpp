#include "stdafx.h"
#include "MarchingCubesNodeController.h"


namespace GEM
{
	bool MarchingCubeController::Initialize()
	{
		try
		{
			m_window = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("MarchingCubesNodeController.layout");
			auto DefWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
			DefWindow->addChild(m_window);
			m_turnedOn = false;
			m_window->deactivate();
			m_window->setVisible(false);

			m_spinner = static_cast<CEGUI::Spinner*>(m_window->getChild("FrameWindow")->getChild("Spinner"));

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
		m_turnedOn = true;
		m_window->activate();
		m_window->setVisible(true);
		CEGUI::System::getSingleton().getDefaultGUIContext().getCursor().setVisible(true);

	}

	void MarchingCubeController::turnOff()
	{
		m_turnedOn = false;
		m_window->deactivate();
		m_window->setVisible(false);
		CEGUI::System::getSingleton().getDefaultGUIContext().getCursor().setVisible(false);
	}

	void MarchingCubeController::PreFrame(float delta)
	{

	}
	void MarchingCubeController::setChosedValue(int val)
	{
		m_spinner->setCurrentValue((double)val);
	}
	int MarchingCubeController::getChosedValue()
	{
		return std::round(m_spinner->getCurrentValue());
	}
	bool MarchingCubeController::isActive()
	{
		return m_turnedOn;
	}
}
