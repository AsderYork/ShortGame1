#pragma once

#include <NetworkCommandsProcessor.h>
#include <GameEventsSystem_Command.h>

namespace GEM {

	class GameEventsSystem_ServerProcessor : public GameSim::NetworkExchangeProcessorSingleCommandSerialization<GameSim::GameEventsSystem_Command>
	{
	public:
		// Унаследовано через NetworkExchangeProcessorSingleCommandSerialization
		inline virtual uint8_t getIdOfProcessor() const { return 3; };

		virtual bool ApplyCommand(const GameSim::NetworkCommand* Command, GameSim::GameTime PacketTime);
	};
}