#include "Mixin_Base.h"
#include "GameSimulation.h"

namespace GEM::GameSim
{
	const GameTime & GEM::GameSim::Mixin_base::getGameTime()
	{
		return m_GameSim->getGameTime();
	}
}