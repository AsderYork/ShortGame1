// Server.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Server.h"

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <chrono>
#include <thread>
#include <cereal\types\chrono.hpp>

namespace GEM {

	

	

	Server::ClientConnectionRep::~ClientConnectionRep()
	{
		if (m_playerTicket.has_value())
		{
			m_server->m_gameSimServer.RemovePlayer(std::move(m_playerTicket.value()));
		}
	}

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

			m_connectionPointer->ClearReciveBuffer();

			auto MaybePlayerTicket = m_server->m_gameSimServer.NewPlayerRoutine(GameSim::Player(m_clientData.ClientName));
			if (!MaybePlayerTicket)
			{
				printf("Connection was successfull, but server is full! So disconnect\n");
				return state::END;
			}
			else
			{
				m_connectionPointer->SendAsArchive(MaybePlayerTicket->get().characterID);
				m_playerTicket.emplace(std::move(MaybePlayerTicket.value()));
				printf("Player added.\n");
			}
			return state::CREATE_PLAYER_AND_WAIT;
		}
		return state::WAIT_FOR_INIT_DATA;
	}
	
	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_CREATE_PLAYER_AND_WAIT()
	{
		std::string StrRep = m_connectionPointer->Recive().str();
		if (m_connectionPointer->Recive().str().size() != 0)
		{
			bool ContinueConnection = false;
			try {				
				m_connectionPointer->ReciveAsArchive(ContinueConnection);
			}
			catch (...)
			{
				return state::ERR;
			}
			m_connectionPointer->ClearReciveBuffer();
			if (!ContinueConnection) { return state::END; }
			
			return state::WORKING;
		}
		return state::CREATE_PLAYER_AND_WAIT;
	}

	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_ERR()
	{
		printf("En error in network communication\n");
		return state::END;
	}
	
	Server::ClientConnectionRep::state Server::ClientConnectionRep::State_WORKING()
	{
		m_connectionPointer->SendAsArchive(std::chrono::system_clock::now(), m_server->m_gameSimServer.GatherDataForPlayer(m_playerTicket->get().id));
		m_server->m_gameSimServer.ReciveSynchroUpdatesFromClient(m_playerTicket->get().id, m_connectionPointer->Recive());
		m_connectionPointer->ClearReciveBuffer();
		return state::WORKING;
	}

	bool Server::ClientConnectionRep::Perform()
	{
		if (!m_connectionPointer->isOpen())
		{
			printf("Player '%s' terminated connection\n", m_clientData.ClientName.c_str());
			m_state = state::END;
			return false;
		}

		m_connectionPointer->ProcessConnection();
		switch (m_state)
		{
		case state::NO_INIT: { m_state = State_NO_INIT(); break; }
		case state::WAIT_FOR_INIT_DATA: { m_state = State_WAIT_FOR_INIT_DATA(); break; }
		case state::CREATE_PLAYER_AND_WAIT: { m_state = State_CREATE_PLAYER_AND_WAIT(); break; }
		case state::WORKING: { m_state = State_WORKING(); break; }

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
			m_playerConnections.emplace_back(std::move(ptr), this);
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
		float Delta = 0.33f;//Expected average for simulation
		while (true)
		{
			auto start = std::chrono::system_clock::now();

			m_gameSimServer.Tick(Delta);
			handleNetwork();

			//std::this_thread::sleep_for(std::chrono::milliseconds(30));
			auto end = std::chrono::system_clock::now();

			Delta = std::chrono::duration_cast<std::chrono::duration<float> >(end - start).count();
		}
	}

}


