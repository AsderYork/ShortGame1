#pragma once
#include "EventBase.h"

#include <cstdint>
#include <vector>
#include <memory>

namespace GEM::GameSim
{
	class SimulationState
	{
		uint64_t StateID;
		
		//Events that where processed during that Simulation tick.
		std::vector<std::unique_ptr<EventBase>> ProcessedEvents;
	};

}