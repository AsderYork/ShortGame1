#include "stdafx.h"
#include "LoginScreen.h"

#include "MainGameScreen.h"

#include <string>         // std::string, std::u32string
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8
#include <cstdint> 
#include <chrono>


namespace GEM
{
	
	void LoginScreen::Init()
	{
		m_mainWindow = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("LoginScreen.layout");
		auto LogWindow = m_mainWindow->getChild("FrameWindow");
		m_editboxUsername = (CEGUI::Editbox*)LogWindow->getChild("Editbox_Username");
		m_editboxAddr = (CEGUI::Editbox*)LogWindow->getChild("Editbox_Addr");
		m_editboxPort = (CEGUI::Editbox*)LogWindow->getChild("Editbox_Port");
		m_lableStatus = (CEGUI::Window*)LogWindow->getChild("StatusLable");

		auto DefWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
		DefWindow->addChild(m_mainWindow);

		LogWindow->getChild("Button")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoginScreen::ButtonPressed, this));

		if (m_stateLineBeginningMessage.size() != 0)
		{
			m_lableStatus->setText(m_stateLineBeginningMessage);
		}

	}

	void LoginScreen::PostFrame(float timeDelta)
	{
		m_PIEG.CheckForEvents();
		auto NewEvent = m_PIEG.getEvent();
		while (NewEvent != nullptr)
		{
			
			switch (NewEvent->getEventID())
			{
			case GameSim::KeyboardEvent_PlayerAttack::id:
			case GameSim::KeyboardEvent_PlayerJump::id:
			case GameSim::KeyboardEvent_PlayerMoveBackward::id:
			case GameSim::KeyboardEvent_PlayerMoveForward::id:
			case GameSim::KeyboardEvent_PlayerMoveLeft::id:
			case GameSim::KeyboardEvent_PlayerMoveRight::id: {
				if (static_cast<GameSim::KeyboardEventBase*>(NewEvent.get())->m_isPressed)
				{
					LOGCATEGORY("LoginScreen/PostFrame").info("NewEvent KeyPressed! %s", NewEvent->getEventTypeName().c_str());
				}
				else
				{
					LOGCATEGORY("LoginScreen/PostFrame").info("NewEvent KeyReleased! %s", NewEvent->getEventTypeName().c_str());
				}
				break;
			}
			default: {
				LOGCATEGORY("LoginScreen/PostFrame").info("NewEvent %s", NewEvent->getEventTypeName().c_str());
				break; }
			}
			m_gameSim->m_gs.InsertEvent(std::move(NewEvent), 0);
			NewEvent = m_PIEG.getEvent();
		}

		if (m_connectionFuture.valid())
		{
			if (m_connectionFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				bool Result;
				try {Result = m_connectionFuture.get();	}
				catch (...)	{Result = false;}
				if (Result)
				{
					std::string Str = "Connected to a server '";
					Str += m_gameClient->getServerData().ServerName + "'.";
					m_lableStatus->setText(Str);

				}
				else
				{
					auto LogWindow = m_mainWindow->getChild("FrameWindow");
					auto Button = (CEGUI::PushButton*)LogWindow->getChild("Button");
					Button->setText("Connect");
					Button->setActive(true);
					m_lableStatus->setText("Failed to connect. Try other data");
				}
			}
		}
		else
		{
			switch (m_gameClient->getState())
			{
			default:
			case NetworkController::state::NOT_CONNECTED:{ break; }
			case NetworkController::state::READY: {
				m_gameSim->ActivateSimulation();
				getController()->AddScreen<MainGameScreen>(m_gameClient, m_gameSim);
				Finish();
				break; }
			
			}
		}
	}

	bool LoginScreen::ButtonPressed(const CEGUI::EventArgs & e)
	{
		/**
		You might ask: hay, what unsigned int is doing here?
		Well, MSVC17 cant handle char32_t for some reason. So this is a recomended solution
		*/
		std::wstring_convert<std::codecvt_utf8<unsigned int>, unsigned int> cdvrt;

		std::string Username = cdvrt.to_bytes((unsigned int*)m_editboxUsername->getText().c_str());
		std::string ServerAddr = cdvrt.to_bytes((unsigned int*)m_editboxAddr->getText().c_str());
		std::string ServerPort = cdvrt.to_bytes((unsigned int*)m_editboxPort->getText().c_str());

		auto LogWindow = m_mainWindow->getChild("FrameWindow");
		auto Button = (CEGUI::PushButton*)LogWindow->getChild("Button");
		Button->setText("connecting");
		Button->setActive(false);
		m_lableStatus->setText("Connecting...");
		
		m_gameClient->SetClientData(ClientData(Username));

		m_connectionFuture = std::async([ServerAddr, ServerPort, this]() {return m_gameClient->Connect(ServerAddr, std::stoi(ServerPort)); });

		return true;
	}

	LoginScreen::~LoginScreen()
	{
		if (m_mainWindow != nullptr)
		{
			CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->removeChild(m_mainWindow);
		}
	}
}