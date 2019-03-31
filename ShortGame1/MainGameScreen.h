#pragma once
#include "ScreenController.h"
#include "GameSimService.h"
#include "SDL2_Service.h"
#include "NetworkController.h"
#include "VisualController.h"
#include "PlayerInputManager.h"

#include "MainCamera.h"
#include "DebugCamera.h"


namespace GEM
{
	class MainGameScreen : public Screen
	{
	private:
		GameSimController* m_gsController;
		NetworkController* m_network;

		VisualController m_visual;
		PlayerInputManager m_inputManager;

		MainCamera m_camera;
		DebugCamera m_debugCamera;

		bool m_basicInitPerformed = false;
		
	public:
		MainGameScreen(NetworkController* network, GameSimController* gs, SDL_Controller* SDL_Cntrlr);
				
		virtual bool Init();

		virtual void PostFrame(float timeDelta) override;


		void CamerasAndControlls(float delta);


		// Унаследовано через Screen
		virtual void OnTop() override;
		virtual void NoLongerOnTop() override;
		virtual void LeaveStack() override;
	};
}