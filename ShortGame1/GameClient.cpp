#include "stdafx.h"
#include "GameClient.h"

#include <cereal\cereal.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>

namespace GEM
{
	bool GameClient::process()
	{
		m_connection->ProcessConnection();
		switch (m_state)
		{
		case state::NOT_CONNECTED: { return false; }
		case state::JUST_CONNECTED: {m_state = s_JUST_CONNECTED(); break; }
		case state::WAIT_FOR_INITIAL_EXCHANGE: {m_state = s_WAIT_FOR_INITIAL_EXCHANGE(); break; }
		case state::READY: {m_state = s_READY(); break; }
		case state::ERROR: {m_state = s_ERROR(); break; }

		default:
		case state::END: {return false; }
		}
		return true;
	}
	GameClient::state GameClient::s_JUST_CONNECTED()
	{
		m_connection->SendAsArchive(m_clientData);

		return state::WAIT_FOR_INITIAL_EXCHANGE;
	}
	GameClient::state GameClient::s_WAIT_FOR_INITIAL_EXCHANGE()
	{
		if (m_connection->Recive().str().size() != 0)
		{
			try {
				m_connection->ReciveAsArchive(m_serverData);
			}
			catch (...)
			{
				return state::ERROR;
			}
			return state::READY;
		}
		return state::WAIT_FOR_INITIAL_EXCHANGE;
	}

	GameClient::state GameClient::s_READY()
	{
		LOGCATEGORY("GameClient/s_READY").info("So connection is established! Servername:%s; But we have nothing to do, so we're close it",
			m_serverData.ServerName.c_str());
		return state::END;
	}

	GameClient::state GameClient::s_ERROR()
	{
		LOGCATEGORY("GameClient/s_ERROR").error("An error occured during communication");
		return state::END;
	}


	bool GameClient::Connect(std::string address, int port)
	{
		try
		{
			m_connection = std::make_unique<NetworkConnection>(std::move(NetworkClient::ProduceConnection(address, port)));
		}
		catch (const std::exception& e)
		{
			LOGCATEGORY("GameClient/Connect").error("Can't make a connection:%s", e.what());
			return false;
		}
		m_state = state::JUST_CONNECTED;
		return true;
	}
	void GameClient::SetClientData(ClientData cd)
	{
		m_clientData = cd;
	}
	ServerData GameClient::getServerData()
	{
		return m_serverData;
	}
}