#pragma once
#include "GameEvent_Base.h"
#include <map>

#include <LogHelper.h>

namespace GEM::GameSim
{
	class EventsFactory
	{
	private:

		static std::map<GameSim::EventIDType, std::function<std::unique_ptr<GameSim::GameEvent>()>> m_eventGenerators;

	public:

		static void RegisterAllEventTypes();

		template<typename EventType>
		static void AddEventType()
		{
			m_eventGenerators.emplace(EventType::staticid(), std::function([]() {return std::make_unique<EventType>(); }));
		}

		static inline std::unique_ptr<GameSim::GameEvent> GenerateEvent(EventIDType id, ObjectIDType Author, GameTime Time) {
			auto EventGen = m_eventGenerators.find(id);
			if (EventGen == m_eventGenerators.end())
			{
				LOGCATEGORY("EventsFactory, GenerateEvent").error("Event %i have no registered generator!");
				return std::unique_ptr<GameSim::GameEvent>();
			}
			auto NewEvent = m_eventGenerators[id]();
			NewEvent->Initialize(Author, Time);
			return NewEvent;
		}

	};
}