#include "stdafx.h"
#include "GameEventsSystem_ClientProcessor.h"
#include <LogHelper.h>

namespace GEM
{
	void GameEventsSystem_ClientProcessor::ConfirmCommand(const GameSim::NetworkCommand* Command)
	{
		auto CastedCommand = static_cast<const GameSim::GameEventsSystem_Command*>(Command);
		

		if (std::holds_alternative<GameSim::GameEventsSystem_Command::InitalPack>(CastedCommand->state))
		{
			//Event definetly must allready be in container. Otherwise someone forgot to put it there!
			GameSim::GameEvent* TrackedEvent = m_trackedEvents[Command->m_uniqueID];

			if (TrackedEvent->m_state == GameSim::GameEvent::Sended)
			{
				if (TrackedEvent->isEventLong)
				{
					TrackedEvent->m_state = GameSim::GameEvent::AwaitingServer;
				}
				else
				{
					TrackedEvent->m_state = GameSim::GameEvent::Completed;
					m_trackedEvents.erase(Command->m_uniqueID);
				}
			}
			else if (TrackedEvent->m_state == GameSim::GameEvent::Stopping)
			{
				if (!TrackedEvent->isEventLong)
				{
					TrackedEvent->m_state = GameSim::GameEvent::Completed;
				}//Otherwise continue stopping
			}
			else
			{
				LOGCATEGORY("GameEventsSystem_ClientProcessor/ConfirmCommand").error("Event is in unexpected state[%i]. And it's confirmation on just sended event.", TrackedEvent->m_state);
			}	
		}
		else
		{
			auto& Refrence = std::get<GameSim::GameEventsSystem_Command::EventRefrence>(CastedCommand->state);

			GameSim::GameEvent* TrackedEvent = m_trackedEvents[Refrence.m_instanceId];

			if (Refrence.isStopRequest)
			{
				m_trackedEvents.erase(Refrence.m_instanceId);
			}
			else
			{
				TrackedEvent->m_state = GameSim::GameEvent::Completed;
				m_trackedEvents.erase(Refrence.m_instanceId);

			}
		}		
		
	}

	void GameEventsSystem_ClientProcessor::RejectCommand(const GameSim::NetworkCommand* Command)
	{
		auto CastedCommand = static_cast<const GameSim::GameEventsSystem_Command*>(Command);

		GameSim::NetworkCommandIDType EventInstanceID;

		if (std::holds_alternative<GameSim::GameEventsSystem_Command::InitalPack>(CastedCommand->state))
		{
			EventInstanceID = Command->m_uniqueID;
		}
		else
		{
			//Then it's a refrence.
			auto& Refrence = std::get<GameSim::GameEventsSystem_Command::EventRefrence>(CastedCommand->state);

			if (Refrence.isStopRequest)
			{
				//If we've got a reject on our stop request, then the event is probably complited. So there is nothing to do
			}
			else
			{
				//Then we rejecting a long command. Well.
				m_trackedEvents[Refrence.m_instanceId]->m_state = GameSim::GameEvent::EventState::Rejected;
				m_trackedEvents.erase(Refrence.m_instanceId);
			}
		}
	}
}

