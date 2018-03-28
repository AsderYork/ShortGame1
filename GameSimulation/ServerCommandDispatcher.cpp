#include "ServerCommandDispatcher.h"

namespace GEM::GameSim
{


	ServerHistory::ServerHistory(ServerCommandDispatcher& dispatcher) : m_processors(dispatcher.m_processors) { }

	void ServerCommandDispatcher::ProcessCommands(std::vector<ClientCommandPack> commandPacks, ServerHistory* History)
	{
		for (auto& commandPack : commandPacks)
		{
			for (auto& command : commandPack.commands)
			{
				History->InjectCommand(std::move(command), commandPack.time);
			}
		}
		History->ReconsiderHistory();
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