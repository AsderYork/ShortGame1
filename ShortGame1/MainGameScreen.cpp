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
		m_visual(gs),
		m_inputManager(&(gs->m_eventsController))
	{
		m_inputManager.RegisterListener(SDL_Cntrlr);
		//m_inputManager.m_eventsController = &m_gsController->m_events;
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
				m_camera.TieCamera(PlayerPtr);
				m_camera.setActive(true);

				m_debugCamera.init();
				m_debugVis.init();

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

			if (m_inputManager.isPlayerEntityAccuried())
			{
				m_inputManager.Apply(timeDelta, m_camera.getOrientation());
			}
			m_visual.Frame(timeDelta);


			CamerasAndControlls(timeDelta);

		}

		if (m_inputManager.ShouldShowDebugOverlay())
		{
			ActivateAnotherScreen(Helper::Hasher<int32_t>::Hash("DebugOverlayScreen", "Screens"));
		}
		

		m_debugVis.frame(timeDelta, m_inputManager.MouseState(), m_inputManager.ButtonHistory());
	}

	void MainGameScreen::CamerasAndControlls(float delta)
	{
		bool ChangeCamera = false;
		bool ChangePlayerControll = false;

		for (auto& press : m_inputManager.ButtonHistory())
		{
			if (press.code == SDL_SCANCODE_F2 && press.is_pressed) {
				ChangeCamera = !ChangeCamera;
			}

			if (press.code == SDL_SCANCODE_F3 && press.is_pressed) {
				ChangePlayerControll = !ChangePlayerControll;
			}

		
		}

		if (ChangeCamera) {
			auto currCamState = m_debugCamera.isActive();
			m_debugCamera.setActive(!currCamState);
			m_camera.setActive(currCamState);
		}

		if (m_debugCamera.isActive())
		{
			if (ChangePlayerControll) {m_inputManager.setProagationState(!m_inputManager.getProagationState());}
		}
		else
		{
			m_inputManager.setProagationState(true);
		}

		if (m_debugCamera.isActive() && SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_F4])
		{
			m_debugCamera.setOrientation(m_camera.getNodeOrientation());
			m_debugCamera.setPosition(m_camera.getPos());

		}

		m_camera.UpdateCamera(delta, m_inputManager.MouseState(), m_inputManager.ButtonHistory());
		m_debugCamera.frame(delta, m_inputManager.MouseState(), m_inputManager.ButtonHistory());
	}

	void MainGameScreen::NoLongerOnTop()
	{
	}
	void MainGameScreen::LeaveStack()
	{
	}

}