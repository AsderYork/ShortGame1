// Server.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Server.h"


namespace GEM {

	

	

	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_NO_INIT()
	{
		std::stringstream OutStre;
		cereal::BinaryInputArchive ar(OutStre);
		ar(m_server->m_serverInfo);
		m_connectionPointer->Send(OutStre);

		return state::WAIT_FOR_INIT_DATA;
	}

	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_WAIT_FOR_INIT_DATA()
	{

		if (m_connectionPointer->Recive().str().size() != 0)
		{
			try {
				cereal::BinaryOutputArchive ar(m_connectionPointer->Recive());
				ar(m_clientData);
			}
			catch (...)
			{
				return state::ERR;
			}

			return state::INITED;
		}
	}

	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_ERR()
	{
		printf("En error in network communication\n");
		return state::END;
	}

	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_INITED()
	{
		printf("Successfully inited. Now what?");
		return state::END;
	}

	bool Server::ClientConnectionRep::Perform()
	{
		switch (m_state)
		{
		case state::NO_INIT: { m_state = State_NO_INIT(); break; }
		case state::WAIT_FOR_INIT_DATA: { m_state = State_WAIT_FOR_INIT_DATA(); break; }
		case state::INITED: { m_state = State_INITED(); break; }

		default:
		case state::ERR: { m_state = State_ERR(); break; }
		case state::END: { return false; }
		}
		return true;
	}

	void Server::handleNetwork()
	{
		m_controller.RunALittle();

		auto ptr = m_controller.GetNewConnectionIfAny();
		while (ptr != nullptr)
		{
			m_playerConnections.emplace_back(ptr, this);
			printf("New connection!\n");
		}

		for (auto player = m_playerConnections.begin(); player != m_playerConnections.end(); player++)
		{
			if (!player->Perform())
			{
				m_playerConnections.erase(player);
			}
		}

	}

	void Server::Run()
	{
		while (true)
		{
			handleNetwork();
		}
	}

}


