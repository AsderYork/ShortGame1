#pragma once
#include "NetworkClient.h"
#include <InformationalStructers.h>

#include <memory>

namespace GEM
{
	class GameClient
	{
	public:
		enum class state {
			NOT_CONNECTED,//No connection is established
			JUST_CONNECTED,//Connection is just established, start initial exchange
			WAIT_FOR_INITIAL_EXCHANGE,//Connection established, wait for initial data exchanged
			READY,//Initial data have been exchange and now link is ready for game transmissions.
			ERROR,//An error occured.
			END//Connection is no more. 
		};
	private:
		std::unique_ptr<NetworkConnection> m_connection;
		ClientData m_clientData;
		ServerData m_serverData;

		
		state m_state = state::NOT_CONNECTED;


		state s_JUST_CONNECTED();
		state s_WAIT_FOR_INITIAL_EXCHANGE();
		state s_READY();
		state s_ERROR();





	public:
		GameClient(ClientData& clientData) : m_clientData(clientData){}

		bool Connect(std::string address, int port);


		/**!
		Processes the connection. Exhanges and all the stuff.
		\returns Returns true, if connection still have something to do, false if all work is done
		*/
		bool process();


	};
}