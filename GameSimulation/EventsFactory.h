#pragma once
#include "GameEvent_Base.h"
#include <map>

namespace GEM::GameSim
{
	class EventsFactory
	{
	private:

		static std::map<GameSim::EventIDType, std::function<std::unique_ptr<GameSim::GameEvent>()>> m_eventGenerators;

	public:

		template<typename EventType>
		static void AddEventType()
		{
			m_eventGenerators.emplace(EventType::staticid(), std::function([]() {return std::make_unique<EventType>(); }));
		}

		static inline std::unique_ptr<GameSim::GameEvent> GenerateEvent(EventIDType id, ObjectIDType Author, GameTime Time) {
			auto NewEvent = m_eventGenerators[id]();
			NewEvent->Initialize(Author, Time);
			return NewEvent;
		}

	};
}