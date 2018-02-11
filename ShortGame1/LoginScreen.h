#pragma once
#include "ScreenController.h"
#include "CEGUI_Service.h"
#include "GameClient.h"
#include <future>

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

		GameClient* m_gameClient;

		std::future<bool> m_connectionFuture;

	public:

		~LoginScreen();

		LoginScreen(GameClient* GameClient) : m_gameClient(GameClient) {}


		void Init() override;

		virtual void PostFrame(float timeDelta);
	};
}