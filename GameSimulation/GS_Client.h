#pragma once
#include "GameSimulation.h"

#include "ClientCommandDispatcher.h"
#include "UpdateSystemClientProcessor.h"
#include "ChunkLoadClientDispatcher.h"
#include "LandscapeSystem_ClientProcessor.h"

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>
#include <chrono>

namespace GEM::GameSim
{
	class GS_Client : public GameSimulation
	{
	private:
		bool m_timeIsSet = false;

	protected:
		ClientCommandDispatcher m_dispatcher;
		UpdateSystemClientProcessor m_updatesProcessor;



	public:
		ChunkLoadClientDispatcher m_chunkDispatcher;

		std::chrono::system_clock::time_point m_lastUpdateTime;
		ENTITY_ID_TYPE m_playerCharacterID;


		inline void Reset() { m_timeIsSet = false; }

		inline void InsertPlayerEvent(std::unique_ptr<EventBase>&& Event) {
			InsertEvent(std::move(Event), m_playerCharacterID);
		}

		inline GS_Client() : m_timeIsSet(false), m_updatesProcessor(this) {
			m_dispatcher.AddProcessor(&m_updatesProcessor);
			m_dispatcher.AddProcessor(&m_chunkDispatcher.getProcessor());
		}

		/**!
		This method is called right after simulation actually started.
		*/
		void SimulationStarted();


		/**!
		Performs one tick of a simulation
		\returns returns true if simmulation should continue ticking, false if something gone worng and simulation should be stopped.
		*/
		bool Tick(float Delta, cereal::BinaryInputArchive& archive, std::stringstream& OutputStream, bool ArchiveIsEmpty=false);
	};
}