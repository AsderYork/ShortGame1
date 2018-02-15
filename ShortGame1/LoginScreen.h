#pragma once
#include "ScreenController.h"
#include "CEGUI_Service.h"
#include "NetworkController.h"
#include "GameSimService.h"
#include <future>
#include <string>

namespace GEM
{
	class LoginScreen : public Screen
	{
	private:
		CEGUI::Window* m_mainWindow;
		CEGUI::Editbox* m_editboxUsername;
		CEGUI::Editbox* m_editboxAddr;
		CEGUI::Editbox* m_editboxPort;
		CEGUI::Window* m_lableStatus;

		bool ButtonPressed(const CEGUI::EventArgs & e);

		NetworkController* m_gameClient;
		GameSimController* m_gameSim;


		std::future<bool> m_connectionFuture;
		std::string m_stateLineBeginningMessage;

	public:

		~LoginScreen();

		LoginScreen(NetworkController* NetworkController, GameSimController* gs, std::string StateLine = "" ) : m_gameClient(NetworkController),
			m_gameSim(gs),
			m_stateLineBeginningMessage(StateLine){}


		void Init() override;

		virtual void PostFrame(float timeDelta);
	};
}