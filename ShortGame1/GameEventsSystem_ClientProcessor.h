#pragma once
#include "NetworkCommandsProcessor.h"
#include "ClientCommandDispatcher.h"
#include "GameEventsSystem_Command.h"

namespace GEM
{

	class GameEventsSystem_ClientProcessor : public GameSim::NetworkExchangeProcessorSingleCommandSerialization<GameSim::GameEventsSystem_Command>
	{
	private:
		//! !!!Something must insert here events, that should be tracked.
		std::map<GameSim::NetworkCommandIDType, GameSim::GameEvent*> m_trackedEvents;

	public:

		inline GameEventsSystem_ClientProcessor() {};


		virtual void ConfirmCommand(const GameSim::NetworkCommand* Command) override;

		virtual void RejectCommand(const GameSim::NetworkCommand* Command) override;

		void addEvent(GameSim::NetworkCommandIDType, GameSim::GameEvent*);

		inline virtual uint8_t getIdOfProcessor() const { return 3; };

	};
}