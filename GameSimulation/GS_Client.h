#pragma once
#include "GameSimulation.h"

namespace GEM::GameSim
{
	class GS_Client
	{
	private:
		GameSimulation m_gs;
	public:

		/**!
		Performs one tick of a simulation
		\returns returns true if simmulation should continue ticking, false if something gone worng and simulation should be stopped.
		*/
		bool Tick(float Delta);
	};
}