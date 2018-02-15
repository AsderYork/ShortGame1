#pragma once
#include <GS_Client.h>
#include "EngineController.h"
#include "NetworkController.h"
#include "GameSimService.h"

namespace GEM
{
	/**!
	Game simulation controller. Controlls everything about simulation.
	The only problem is that it doesn't know when connection it's connected to a server and when it isn't.
	For that purpose, simulation have two methods, that can start simulation and turn it off
	*/
	class GameSimController : public GameSim::GS_Client, public Service
	{
	private:
		NetworkController * m_network;
		bool m_simmulationActive = false;
	public:
		GameSimController(NetworkController* network) : m_network(network){}
		
		/**!
		Activates simulation. This method should only be called, when provided network service is READY.
		If something something happens connection dies during simmulation, it will become inactive.
		*/
		void ActivateSimulation();

		/**!
		Stops the simulation and completely resets it's state.
		*/
		void DeactivateSimmulation();

		/**!
		Returns true if simulation is active, false otherwise
		*/
		inline bool getSimulationState() { return m_simmulationActive; }


		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(float timeDelta) override;
		virtual ActionResult frame(float timeDelta) override;
		virtual ActionResult postFrame(float timeDelta) override;
	};
}