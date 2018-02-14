#pragma once
#include "GameSimulation.h"
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>

namespace GEM::GameSim
{
	class GS_Client
	{
	private:
	public:
		GameSimulation m_gs;

		/**!
		Performs one tick of a simulation
		\returns returns true if simmulation should continue ticking, false if something gone worng and simulation should be stopped.
		*/
		bool Tick(float Delta, cereal::BinaryInputArchive& archive);
	};
}