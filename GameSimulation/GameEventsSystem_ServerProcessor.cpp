#include "GameEventsSystem_ServerProcessor.h"
#include "EventsFactory.h"

#include <algorithm>

namespace GEM::GameSim
{
	std::weak_ptr<EntityBase> GameEventsSystem_ServerProcessor::AquireObject(ENTITY_ID_TYPE ID) {
		//TODO: Decide weather check, if player can actually perform events on this object is required, or if it's CheckValidiy's job
		
		return m_entConntroller->GetEntity(ID);	

	}


	bool GameEventsSystem_ServerProcessor::ApplyCommand(const NetworkCommand* Command, GameTime PacketTime) {
		auto CastedCommand = static_cast<const GameEventsSystem_Command*>(Command);

		if (std::holds_alternative<GameEventsSystem_Command::InitalPack>(CastedCommand->state))
		{
			auto InitialPack = std::get<GameEventsSystem_Command::InitalPack>(CastedCommand->state);
			auto NewEvent = EventsFactory::GenerateEvent(InitialPack.m_eventId, InitialPack.m_AuthorID, InitialPack.m_CreationTime);

			if (NewEvent == nullptr)
			{
				LOGCATEGORY("GameEventsSystem_ServerProcessor::ApplyCommand").error("Failed to create event!");
				return false;
			}
			
			if (NewEvent->isEventLong())
			{
				auto PlayerEventList = m_activeEvents.find(getCurrentPlayer());
				if (PlayerEventList == m_activeEvents.end())
				{
					LOGCATEGORY("GameEventsSystem_ServerProcessor/ApplyCommand").error("En event have been recived from an unregistered player!");
					return false;
				}
				PlayerEventList->second.emplace_back(EventHolder{ std::move(NewEvent), CastedCommand->m_uniqueID });

				return true;
			}
			else
			{
				auto ThisObjWeakPtr = AquireObject(InitialPack.m_AuthorID);
				if (ThisObjWeakPtr.expired())
				{
					LOGCATEGORY("GameEventsSystem_ServerProcessor/ApplyCommand").error("Author object cannot be found!");
					return false;
				}

				auto ThisObj = ThisObjWeakPtr.lock();
				auto deps = GameEvent::Dependecies(&(*ThisObj), m_entConntroller);
				auto CheckResult = NewEvent->CheckValidity(deps);
				if (CheckResult.has_value() && CheckResult.value())
				{
					NewEvent->ApplyEvent(deps);
					return true;
				}
				return false;

			}
		}
		else if (std::holds_alternative<GameEventsSystem_Command::EventRefrence>(CastedCommand->state))
		{//If cliend sent us a refrence, it can only be a request for event removal.
			auto EventRef = std::get<GameEventsSystem_Command::EventRefrence>(CastedCommand->state);
			

			auto PlayerEventList = m_activeEvents.find(getCurrentPlayer());
			if (PlayerEventList == m_activeEvents.end())
			{
				LOGCATEGORY("GameEventsSystem_ServerProcessor/ApplyCommand").error("En event have been recived from an unregistered player!");
				return false;
			}


			auto RefrencedEvent = std::lower_bound(PlayerEventList->second.begin(), PlayerEventList->second.end(), EventRef.m_instanceId, [](const EventHolder& left, const NetworkCommandIDType& right) {return left.m_eventid < right; });
			
			if (RefrencedEvent == PlayerEventList->second.end() || RefrencedEvent->m_eventid != EventRef.m_instanceId)
			{//Maybe this event is allready executed? So just return false;
				return false;
			}
			
			PlayerEventList->second.erase(RefrencedEvent);
			return true;
		
		}
	}

	void GameEventsSystem_ServerProcessor::newPlayerAdded(PLAYER_ID_TYPE newPlayerID) {
		if (m_activeEvents.find(newPlayerID) != m_activeEvents.end())
		{
			LOGCATEGORY("GameEventsSystem_ServerProcessor/newPlayerAdded").error("Player with this ID is allready registered!");
			return;
		}

		m_activeEvents.emplace( newPlayerID, std::list<EventHolder>());
	}

	/**
	This method must be called every time a player is removed
	*/
	void GameEventsSystem_ServerProcessor::playerRemoved(PLAYER_ID_TYPE removedPlayerID) {

		if (m_activeEvents.find(removedPlayerID) == m_activeEvents.end())
		{
			LOGCATEGORY("GameEventsSystem_ServerProcessor/newPlayerAdded").error("Player with this ID is not registered!");
			return;
		}

		m_activeEvents.erase(removedPlayerID);

	}


}

