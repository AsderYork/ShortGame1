#pragma once
#include "NetworkCommandsProcessor.h"
#include "GameSimulation.h"
#include "ClientCommandDispatcher.h"
#include "GameTimeSystem_Command.h"

namespace GEM::GameSim
{

	class GameTimeSystem_ClientProcessor : public NetworkExchangeProcessorSingleCommandSerialization<GameTimeSystemCommand>
	{
	private:

		GameSimulation * m_gameSim;

	public:

		inline GameTimeSystem_ClientProcessor(GameSimulation* GameSim) : m_gameSim(GameSim) {}


		// Унаследовано через NetworkExchangeProcessor
		virtual bool ApplyCommand(const NetworkCommand * Command, GameTime PacketTime) override;

		inline virtual uint8_t getIdOfProcessor() const { return 2; };

	};
}