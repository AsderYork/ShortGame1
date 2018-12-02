#include "EventsFactory.h"
#include "Object_Player.h"

namespace GEM::GameSim
{
	std::map<GameSim::EventIDType, std::function<std::unique_ptr<GameSim::GameEvent>()>> EventsFactory::m_eventGenerators;

	void EventsFactory::RegisterAllEventTypes()
	{
		EventsFactory::AddEventType<Object_Player_DeathBlast>();
		EventsFactory::AddEventType<Object_Player_Heal>();
	}
}