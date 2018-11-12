#include "EventsFactory.h"
namespace GEM::GameSim
{
	std::map<GameSim::EventIDType, std::function<std::unique_ptr<GameSim::GameEvent>()>> EventsFactory::m_eventGenerators;
}