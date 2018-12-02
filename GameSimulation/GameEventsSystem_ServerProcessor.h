#pragma once
#include "GameEventsSystem_Command.h"
#include "NetworkCommandsProcessor.h"

#include <cereal/archives/binary.hpp>

namespace GEM::GameSim {

	class GameEventsSystem_ServerProcessor : public GameSim::NetworkExchangeProcessorSingleCommandSerialization<GameSim::GameEventsSystem_Command>
	{
	private:
		struct EventHolder {
			std::unique_ptr<GameEvent> m_event;
			NetworkCommandIDType m_eventid;
		};

		//It is important, who exactly sent the event, becouse event unique number is unique only in one session!
		std::map<PLAYER_ID_TYPE, std::list<EventHolder> > m_activeEvents;

		EntityController *m_entConntroller;

		std::weak_ptr<EntityBase> AquireObject(ENTITY_ID_TYPE ID);

	public:

		inline GameEventsSystem_ServerProcessor(EntityController* entController) : m_entConntroller(entController) {
		}

		bool ApplyCommand(const GameSim::NetworkCommand* Command, GameSim::GameTime PacketTime) override;

		inline virtual uint8_t getIdOfProcessor() const { return 3; };

		/**
		This method must be called every time new player is added.
		*/
		void newPlayerAdded(PLAYER_ID_TYPE newPlayerID);

		/**
		This method must be called every time a player is removed
		*/
		void playerRemoved(PLAYER_ID_TYPE removedPlayerID);

	};
}