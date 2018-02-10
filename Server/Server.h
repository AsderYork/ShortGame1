#pragma once


#include <GameSimulation.h>
#include <Mixin_Controller.h>

#include <InformationalStructers.h>


#include "ConnectionController.h"
#include <list>



namespace GEM
{
	class Server
	{
	public:
		class ClientConnectionRep;
	private:
		GEM::ConnectionController m_controller;
		GEM::GameSim::GameSimulation m_gameSim;

		GEM::ServerData m_serverInfo;

		std::list<ClientConnectionRep> m_playerConnections;

	public:

		class ClientConnectionRep
		{
		public:
			enum class state { NO_INIT, WAIT_FOR_INIT_DATA, INITED, ERR, END };
		private:
			GEM::NetworkConnection* m_connectionPointer;
			state m_state;
			Server* m_server;

			ClientData m_clientData;

		public:

			ClientConnectionRep(GEM::NetworkConnection* ptr, Server* server) : m_connectionPointer(ptr),
				m_state(state::NO_INIT), m_server(server) {}

			state State_NO_INIT();

			state State_WAIT_FOR_INIT_DATA();

			state State_ERR();

			state State_INITED();

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