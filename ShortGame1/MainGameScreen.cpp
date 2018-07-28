#include "stdafx.h"
#include "MainGameScreen.h"
#include "LoginScreen.h"

namespace GEM
{
	MainGameScreen::MainGameScreen(NetworkController * network, GameSimController * gs) : m_network(network), m_gsController(gs), m_visual(gs)
	{
	}


	void MainGameScreen::IsOnTop()
	{
		if (m_basicInitPerformed)//Check if it's the first time this screen gets on top
		{
			m_visual.WaitForInit();//If so, perform init
			m_basicInitPerformed = true;
		}
	}

	void MainGameScreen::Init()
	{
		m_visual.StartBackgroundInit();
	}
	void MainGameScreen::PostFrame(float timeDelta)
	{
		if (!m_gsController->getSimulationState())
		{
			m_network->Disconnect();
			LOGCATEGORY("MainGameScreen/PostFrame").info("Simmulation is stopped!");
			getController()->AddScreen<LoginScreen>(m_network, m_gsController, "Simulation were terminated. Try to reconnect");
			Finish();
		}
		else
		{
			//If simulation is really working still

			m_PIEG.CheckForEvents();
			auto NewEvent = m_PIEG.getEvent();
			while (NewEvent != nullptr)
			{						
				m_gsController->InsertPlayerEvent(std::move(NewEvent));
				NewEvent = m_PIEG.getEvent();
			}

			m_visual.Frame();
		}
		
	}
}