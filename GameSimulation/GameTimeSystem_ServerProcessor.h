#pragma once
#include "NetworkCommandsProcessor.h"
#include "GameSimulation.h"
#include "ClientCommandDispatcher.h"
#include "GameTimeSystem_Command.h"

namespace GEM::GameSim
{

	class GameTimeSystem_ServerProcessor : public NetworkExchangeProcessorSingleCommandSerialization<GameTimeSystemCommand>
	{
	private:

		GameSimulation * m_gameSim;

	public:

		inline GameTimeSystem_ServerProcessor(GameSimulation* GameSim) : m_gameSim(GameSim) {}
		inline uint8_t getIdOfProcessor() const { return 2; }

	};
}