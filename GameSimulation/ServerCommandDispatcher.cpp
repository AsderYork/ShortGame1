#include "ServerCommandDispatcher.h"

namespace GEM::GameSim
{


	ServerHistory::ServerHistory(ServerCommandDispatcher& dispatcher) : m_processors(dispatcher.m_processors), m_lastRecivedCommand(0) { }

	void ServerCommandDispatcher::ProcessCommands(std::vector<ClientCommandPack> commandPacks, ServerHistory* History)
	{
		for (auto& commandPack : commandPacks)
		{
			for (auto& command : commandPack.commands)
			{
				History->InjectCommand(std::move(command), commandPack.time);
			}
		}
	}

	ServerCommandPack ServerCommandDispatcher::GatherResults(ServerHistory* History, GameTime CurrentTime)
	{
		ServerCommandPack newCommandPack;
		newCommandPack.time = CurrentTime;
		newCommandPack.commands.swap(History->m_commandsToSend);
		newCommandPack.HistoryPack = History->getHistoryPack();
		return newCommandPack;
	}
}