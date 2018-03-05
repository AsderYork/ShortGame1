#pragma once
#include "GameSimulation.h"
#include "GameHistory.h"
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>
#include <chrono>

namespace GEM::GameSim
{
	class GS_Client
	{
	private:
		bool m_timeIsSet = false;

	protected:
		ENTITY_ID_TYPE m_playerCharacterID;
		GameHistoryController m_gameHistory;


	public:
		GameSimulation m_gs;
		std::chrono::system_clock::time_point m_lastUpdateTime;

		inline void InsertPlayerEvent(std::unique_ptr<EventBase>&& Event) {
			m_gs.InsertEvent(std::move(Event), m_playerCharacterID);
		}

		inline void Reset() { m_timeIsSet = false; }




		/**!
		Performs one tick of a simulation
		\returns returns true if simmulation should continue ticking, false if something gone worng and simulation should be stopped.
		*/
		bool Tick(float Delta, cereal::BinaryInputArchive& archive, std::stringstream& OutputStream, bool ArchiveIsEmpty=false);
	};
}