#include "stdafx.h"
#include "NetworkController.h"

#include <cereal\cereal.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>

namespace GEM
{
	bool NetworkController::process()
	{
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

	Service::ActionResult NetworkController::initialize()
	{
		return ActionResult::AR_OK;
	}
	void NetworkController::shutdown()
	{
		m_state = state::END;
		m_connection.reset();
	}
	Service::ActionResult NetworkController::preFrame(float timeDelta)
	{
		process();
		return ActionResult::AR_OK;
	}
	Service::ActionResult NetworkController::frame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}
	Service::ActionResult NetworkController::postFrame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}


	NetworkController::state NetworkController::s_JUST_CONNECTED()
	{
		m_connection->ProcessConnection();
		m_connection->SendAsArchive(m_clientData);

		return state::WAIT_FOR_INITIAL_EXCHANGE;
	}
	NetworkController::state NetworkController::s_WAIT_FOR_INITIAL_EXCHANGE()
	{
		m_connection->ProcessConnection();
		if (m_connection->Recive().str().size() != 0)
		{
			try {
				m_connection->ReciveAsArchive(m_serverData);
			}
			catch (...)
			{
				return state::ERROR;
			}

			LOGCATEGORY("NetworkController/s_WAIT_FOR_INITIAL_EXCHANGE").info("So connection is established! Servername:'%s';",
				m_serverData.ServerName.c_str());
			return state::READY;
		}
		return state::WAIT_FOR_INITIAL_EXCHANGE;
	}

	NetworkController::state NetworkController::s_READY()
	{
		m_connection->Recive().str(std::string());
		m_connection->ProcessConnection();
		return state::READY;
	}

	NetworkController::state NetworkController::s_ERROR()
	{
		LOGCATEGORY("NetworkController/s_ERROR").error("An error occured during communication");
		return state::END;
	}


	bool NetworkController::Connect(std::string address, int port)
	{
		try
		{
			m_connection = std::make_unique<NetworkConnection>(std::move(NetworkClient::ProduceConnection(address, port)));
		}
		catch (const std::exception& e)
		{
			LOGCATEGORY("NetworkController/Connect").error("Can't make a connection:%s", e.what());
			return false;
		}
		m_state = state::JUST_CONNECTED;
		return true;
	}
	void NetworkController::SetClientData(ClientData cd)
	{
		m_clientData = cd;
	}
	ServerData NetworkController::getServerData()
	{
		return m_serverData;
	}
	NetworkConnection * NetworkController::getConnection()
	{
		if (m_state != state::READY) { LOGCATEGORY("NetworkController/getConnection").error("getConnection is called for a connection, that isn't ready!");
		return nullptr;
		}
		return m_connection.get();
	}
}