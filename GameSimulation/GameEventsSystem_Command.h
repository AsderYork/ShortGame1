#pragma once
#include "NetworkCommandBase.h"
#include "GameEvent_Base.h"
#include "GameTime.h"
#include <variant>
#include <LogHelper.h>

namespace GEM::GameSim
{
	class GameEventsSystem_Command : public NetworkCommand
	{
	public:

		/**
		Refrence to some event that is already sended.
		If this refrence is sended from client to server,
		that means client wants to stop the event,
		if it is sended from server to client, that means server confirms the event.
		*/
		struct EventRefrence
		{
			NetworkCommandIDType m_instanceId;
			bool isStopRequest;//If not rejection, then confirmation.
		};

		/**
		Initial block of data to send to the server.
		*/
		struct InitalPack
		{
			ObjectIDType m_AuthorID;
			GameTime m_CreationTime;
			EventIDType m_eventId;

		};



		std::variant<EventRefrence, InitalPack> state;


		GameEventsSystem_Command(NetworkCommandIDType BaseEventInstance, bool isStopRequest) :
			NetworkCommand(3),
			state(EventRefrence{ BaseEventInstance, isStopRequest })
		{}

		GameEventsSystem_Command(const GameEvent* BaseEvent) :
			NetworkCommand(3),
			state(InitalPack{ BaseEvent->getAuthorID(), BaseEvent->getCreationType(), BaseEvent->id() })
		{}

		GameEventsSystem_Command() :
			NetworkCommand(3)
		{}

		template<class Archive>
		void save(Archive & archive) const
		{
			bool IsInitial;
			if (std::holds_alternative<InitalPack>(state))
			{
				IsInitial = true;
				auto& Data = std::get<InitalPack>(state);
				archive(IsInitial);
				archive(Data.m_AuthorID);
				archive(Data.m_CreationTime);
				archive(Data.m_eventId);
			}
			else if (std::holds_alternative<EventRefrence>(state))
			{
				IsInitial = false;
				archive(IsInitial);
				auto& Data = std::get<EventRefrence>(state);
				archive(Data.m_instanceId);
				archive(Data.isStopRequest);
			}
			else
			{
				LOGCATEGORY("GameEventsSystem_Command/save").error("Command[%PRIu64] state is invalid!", m_uniqueID);
			}
		}

		template<class Archive>
		void load(Archive & archive)
		{
			bool IsInitial;
			archive(IsInitial);
			if(IsInitial)
			{
				InitalPack Data;
				archive(Data.m_AuthorID);
				archive(Data.m_CreationTime);
				archive(Data.m_eventId);

				state = Data;
			}
			else
			{
				EventRefrence Data;
				archive(Data.m_instanceId);
				archive(Data.isStopRequest);

				state = Data;

			}
		}
	};
}