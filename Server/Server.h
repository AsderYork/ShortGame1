#pragma once


#include <GS_Server.h>
#include <Mixin_Controller.h>

#include <InformationalStructers.h>


#include "ConnectionController.h"
#include <list>
#include <memory>


namespace GEM
{
	class Server
	{
	public:
		class ClientConnectionRep;
	private:
		GEM::ConnectionController m_controller;
		GameSim::GS_Server m_gameSimServer;

		GEM::ServerData m_serverInfo;

		std::list<ClientConnectionRep> m_playerConnections;

	public:

		class ClientConnectionRep
		{
		public:
			enum class state { NO_INIT,//Connection is not inited
				WAIT_FOR_INIT_DATA,//ServerData is sent, wait for PlayerData
				CREATE_PLAYER_AND_WAIT,//PlayerData recived. Create new player and wait till the player is ready to start.
				WORKING,//Main game loop. At this point, GameSim controlls send and recive.
				ERR,//Something got wrong
				END };//Connection is terminated and should be removed.
		private:
			std::unique_ptr<NetworkConnection> m_connectionPointer;
			state m_state;
			Server* m_server;

			ClientData m_clientData;
			std::optional<GameSim::PlayerTicket> m_playerTicket;

		public:

			ClientConnectionRep(std::unique_ptr<GEM::NetworkConnection> ptr, Server* server) : m_connectionPointer(std::move(ptr)),
				m_state(state::NO_INIT), m_server(server) {}
			~ClientConnectionRep();

			state State_NO_INIT();
			state State_WAIT_FOR_INIT_DATA();
			state State_CREATE_PLAYER_AND_WAIT();
			state State_ERR();
			state State_WORKING();


			/**!
			Return false, if all work is done and connection should be deleated. true if there is still a work to do.
			*/
			bool Perform();

		};
		friend class ClientConnectionRep;


		Server(int Port, ServerData Serverinfo) : m_controller(Port), m_serverInfo(Serverinfo) {}



		void handleNetwork();

		void Run();


	};
}