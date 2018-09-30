#pragma once
#include <GameSimulation.h>

#include <ClientCommandDispatcher.h>
#include <UpdateSystemClientProcessor.h>
#include "ChunkLoadClientDispatcher.h"
#include "LandscapeSystem_ClientProcessor.h"
#include <GameTimeSystem_ClientProcessor.h>

#include <LandscapePhysics.h>

#include <NetworkConnection.h>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>
#include <chrono>
#include <functional>
#include <vector>

namespace GEM
{
	class GS_Client : public GameSim::GameSimulation
	{
	private:
		bool m_timeIsSet = false;
		bool m_stateInitialized = false;

	protected:
		GameSim::ClientCommandDispatcher m_dispatcher;
		GameSim::UpdateSystemClientProcessor m_updatesProcessor;
		GameSim::GameTimeSystem_ClientProcessor m_gameTimeProcessor;
		
		std::vector<std::function<bool()>> m_firstTickEventCallbacks;


	public:
		ChunkLoadClientDispatcher m_chunkDispatcher;
		GameSim::LandscapePhysics m_landPhys;

		std::chrono::system_clock::time_point m_lastUpdateTime;
		GameSim::ENTITY_ID_TYPE m_playerCharacterID;


		inline void Reset() { m_timeIsSet = false; }

		
		GS_Client();

		/**!
		This method is called right after simulation actually started.
		*/
		void SimulationStarted();


		/**!
		Performs one tick of a simulation
		\returns returns true if simmulation should continue ticking, false if something gone worng and simulation should be stopped.
		*/
		bool Tick(float Delta, GEM::NetworkConnection* connection);

		/**!
		Client's GameSim is very dependant on Server's state, which is avaliable only after reciving first server package.
		Before this package arrived and processed, GS_Client is basicaly uninitialized, and kinda useless.
		That's why any system can register it's callback, that will be called right after the first packed is
		recived and gameState is initialized!
		This event will occur only once, so regitering a callback after this event happened is an error!

		Functions should return a true if everything is ok, and false if there
		is an error and gs_client should stop.

		*/
		inline void AddFirstTickCallback(std::function<bool()> newReaction)
		{
			if(m_stateInitialized)
			{
				LOGCATEGORY("GS_Client/AddFirstTickCallback").error("Something attempted to register a callback after first tick!");
				return;
			}
			m_firstTickEventCallbacks.push_back(newReaction);
		}

		void SimulationShutdown();

		inline bool isStateInited() { return m_stateInitialized; }
	};


}