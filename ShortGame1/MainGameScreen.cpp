#include "stdafx.h"
#include "MainGameScreen.h"
#include "LoginScreen.h"

#include <Hasher.h>
#include <chrono>

namespace GEM
{
	MainGameScreen::MainGameScreen(NetworkController * network, GameSimController * gs, SDL_Controller* SDL_Cntrlr) :
		m_network(network),
		m_gsController(gs),
		m_visual(gs)
	{
		m_inputManager.RegisterListener(SDL_Cntrlr);
	}


	void MainGameScreen::OnTop()
	{
		if (!m_basicInitPerformed)//Check if it's the first time this screen gets on top
		{
			m_gsController->AddFirstTickCallback([this]() {
				auto PlayerPtr = m_gsController->m_entities.GetEntity(m_gsController->m_playerCharacterID);
				if (PlayerPtr.expired())
				{
					LOGCATEGORY("MainGameScreen/WaitForInitCallback").error("Can't tie to a player! Entity is invalid");
					return false;
				}
				m_inputManager.AccuirePlayerEntity(PlayerPtr);
				return true;
			});
			m_visual.WaitForInit();//If so, perform init
			m_basicInitPerformed = true;
		}
	}

	bool MainGameScreen::Init()
	{
		m_visual.StartBackgroundInit();
		return true;
	}
	void MainGameScreen::PostFrame(float timeDelta)
	{

		if (!m_gsController->getSimulationState())
		{
			m_network->Disconnect();
			LOGCATEGORY("MainGameScreen/PostFrame").info("Simmulation is stopped!");
			MarkForRemoval();
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

			if (m_inputManager.isPlayerEntityAccuried())
			{
				m_inputManager.Apply(timeDelta);
			}
			m_visual.Frame(timeDelta);
		}

		if (m_inputManager.ShouldShowDebugOverlay())
		{
			ActivateAnotherScreen(Helper::Hasher<int32_t>::Hash("DebugOverlayScreen", "Screens"));
		}
		
	}

	void MainGameScreen::NoLongerOnTop()
	{
	}
	void MainGameScreen::LeaveStack()
	{
	}
}