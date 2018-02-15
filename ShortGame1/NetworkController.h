#pragma once
#include "NetworkClient.h"
#include "EngineController.h"
#include <InformationalStructers.h>

#include <memory>

namespace GEM
{
	class NetworkController : public Service
	{
	public:
		enum class state {
			NOT_CONNECTED,//No connection is established
			CONNECTION_STARTED,//No connection, but we have send a request for one
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

		bool Connect(std::string address, int port);
		inline void Disconnect() { m_connection.reset(); m_state = state::END; }

		void SetClientData(ClientData cd);
		ServerData getServerData();

		/**!
		Returns the connection if there is any.
		This method should only be called for connections, that are READY.
		*/
		NetworkConnection* getConnection();

		inline state getState() const { return m_state; }


		/**!
		Processes the connection. Exhanges and all the stuff.
		\returns Returns true, if connection still have something to do, false if all work is done
		*/
		bool process();



		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(float timeDelta) override;
		virtual ActionResult frame(float timeDelta) override;
		virtual ActionResult postFrame(float timeDelta) override;

	};
}