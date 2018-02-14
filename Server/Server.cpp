// Server.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Server.h"

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>

#include <GS_Server.h>
#include <GS_Client.h>

namespace GEM {

	

	

	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_NO_INIT()
	{
		printf("New connection!\n");
		m_connectionPointer->SendAsArchive(m_server->m_serverInfo);

		printf("Data send. Wait for response\n");
		return state::WAIT_FOR_INIT_DATA;
	}

	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_WAIT_FOR_INIT_DATA()
	{

		std::string StrRep = m_connectionPointer->Recive().str();
		if (m_connectionPointer->Recive().str().size() != 0)
		{
			try {
				m_connectionPointer->ReciveAsArchive(m_clientData);
			}
			catch (...)
			{
				return state::ERR;
			}
			printf("Player named %s is connected\n", m_clientData.ClientName.c_str());
			return state::INITED;
		}
		return state::WAIT_FOR_INIT_DATA;
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
			ptr = m_controller.GetNewConnectionIfAny();
		}

		std::vector<std::list<ClientConnectionRep>::iterator> PlayersToRemove;
		for (auto player = m_playerConnections.begin(); player != m_playerConnections.end(); player++)
		{
			if (!player->Perform())
			{
				PlayersToRemove.push_back(player);
			}
		}
		for(auto& p : PlayersToRemove) {m_playerConnections.erase(p);}

	}

	void Server::Run()
	{
		GameSim::GS_Server Srv;
		GameSim::GS_Client Client;

		auto Pl = Srv.NewPlayerRoutine(GEM::GameSim::Player("Jhoanna"));
		

		while (true)
		{
			Srv.Tick(1.0f);

			std::stringstream strstr;
			{
				cereal::BinaryOutputArchive ar(strstr);
				ar(Srv.GetUpdatesForPlayer(Pl->get().id));

				Srv.GetUpdatesForPlayer(Pl->get().id).clear();
			}
			cereal::BinaryInputArchive ar(strstr);
			Client.Tick(1.0f, ar);

			//handleNetwork();
		}
	}

}


