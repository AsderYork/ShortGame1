#include "stdafx.h"
#include "EventsController.h"
#include <GameEventsSystem_Command.h>
#include "GameEventsSystem_ClientProcessor.h"

#include <LogHelper.h>

#include <Object_Player.h>


namespace GEM
{
	GameEventsController::EventTicket&& GameEventsController::AddEvent(GameSim::EventIDType EventID, GameSim::ENTITY_ID_TYPE authorId)
	{
		auto newEvent = GameSim::EventsFactory::GenerateEvent(EventID, authorId, *m_gameTime);
		auto ReturnTicket = EventTicket(newEvent.get());
		m_eventsList.push_back({ std::move(newEvent), 0 });
		return std::move(ReturnTicket);
	}

	void GameEventsController::ProcessEvents()
	{
		for (auto& [Event, network_id] : m_eventsList)
		{
			if (Event->m_state == GameSim::GameEvent::EventState::Completed || Event->m_state == GameSim::GameEvent::EventState::Rejected)
			{//Then this event should be removed
				if (Event->m_refCounter == 0)
				{
					Event.reset();
				}
			}
			else if (Event->m_state == GameSim::GameEvent::EventState::Started)
			{
				auto cmd = std::make_unique<GameSim::GameEventsSystem_Command>(Event.get());
				auto cmdptr = cmd.get();
				m_commandDispatcher->InsertPerformedCommand(std::move(cmd));
				Event->m_state = GameSim::GameEvent::EventState::Sended;
				network_id = cmdptr->m_uniqueID;
				m_eventsClientProcessor->addEvent(network_id, Event.get());
			}
			else if (Event->m_state == GameSim::GameEvent::EventState::Stopping) {
				m_commandDispatcher->InsertPerformedCommand(std::make_unique<GameSim::NetworkCommand>(GameSim::GameEventsSystem_Command(network_id, true)));
			}
			/*Events now are not performed on client at all. They are initialized on client, and then confirmed on the server.
			Server just updates gamestate accordingly and warns client about event confirmation.*/			
		}
		m_eventsList.remove_if([](auto& val) {return val.m_event == nullptr; });
	}
}