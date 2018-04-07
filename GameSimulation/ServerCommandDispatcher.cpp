#include "ServerCommandDispatcher.h"

namespace GEM::GameSim
{
	void ServerHistory::InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime)
	{
		if (command->m_uniqueID == 0) { return; }//Commands with id 0 don't use histories
		m_lastRecivedCommand = command->m_uniqueID;

		if (!m_processors[command->m_header]->ApplyCommand(command.get(), PacketTime))
		{
			m_rejectedCommands.push_back(m_lastRecivedCommand);
		}
	}
	ServerHistory::ServerHistory(ServerCommandDispatcher& dispatcher) : m_processors(dispatcher.m_processors), m_lastRecivedCommand(1) { }

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