#include "stdafx.h"
#include "DebugOverlayScreen.h"

namespace GEM
{
	bool DebugOverlayScreen::Init()
	{
		m_mainWindow = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("DebugOverlay.layout");
		m_text = m_mainWindow->getChild("Label");


		m_mainWindow->deactivate();
		m_mainWindow->setVisible(false);
		auto DefWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
		DefWindow->addChild(m_mainWindow);
		return true;
	}

	void DebugOverlayScreen::OnTop()
	{
		m_mainWindow->activate();
		m_mainWindow->setVisible(true);
	}

	void DebugOverlayScreen::NoLongerOnTop()
	{
		m_mainWindow->setVisible(false);
		m_mainWindow->deactivate();
	}

	void DebugOverlayScreen::LeaveStack()
	{
	}

	void DebugOverlayScreen::PostFrame(float Delta)
	{
		m_timePassed += Delta;
		m_framesPassed++;
		if (m_timePassed > 1.0f)
		{
			auto FPS = (float)m_framesPassed / m_timePassed;
			m_text->setText("FPS:" + std::to_string(FPS) + "\n");
			m_framesPassed = 0;
			m_timePassed = 0;
		}



		if (m_listener.m_shouldClose)
		{
			MarkForRemoval();
		}
	}

	void DebugOverlayScreen::InputListener::textInput(const SDL_TextInputEvent & arg)
	{
	}
	void DebugOverlayScreen::InputListener::keyPressed(const SDL_KeyboardEvent & arg)
	{
		
	}
	void DebugOverlayScreen::InputListener::keyReleased(const SDL_KeyboardEvent & arg)
	{
	}
}
