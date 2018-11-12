#pragma once
#include "GameEventsSystem_Command.h"
#include "NetworkCommandsProcessor.h"

#include <cereal/archives/binary.hpp>

namespace GEM::GameSim {

	class GameEventsSystem_ServerProcessor : public GameSim::NetworkExchangeProcessorSingleCommandSerialization<GameSim::GameEventsSystem_Command>
	{
	public:

		bool ApplyCommand(const GameSim::NetworkCommand* Command, GameSim::GameTime PacketTime) override;

		inline virtual uint8_t getIdOfProcessor() const { return 3; };

	};
}