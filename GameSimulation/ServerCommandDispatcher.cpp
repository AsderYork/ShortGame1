#include "ServerCommandDispatcher.h"

namespace GEM::GameSim
{
	void ServerHistory::InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime, GameTime CurrTime)
	{
		if (command->m_uniqueID == 0) { return; }//Commands with id 0 don't use histories
		m_lastRecivedCommand = command->m_uniqueID;
		
		m_processors[command->m_header]->setCurrentPlayer(m_playerID);
		if (!m_processors[command->m_header]->ApplyCommand(command.get(), (CurrTime - PacketTime) / 2))
		{
			m_rejectedCommands.push_back(m_lastRecivedCommand);
		}
	}
	ServerHistory::ServerHistory(ServerCommandDispatcher& dispatcher, PLAYER_ID_TYPE playerID) : m_processors(dispatcher.m_processors),
		m_lastRecivedCommand(0), m_playerID(playerID) { }

	void ServerCommandDispatcher::ProcessCommands(std::vector<ClientCommandPack_Server> commandPacks, ServerHistory* History, GameTime CurrentTime)
	{
		for (auto& commandPack : commandPacks)
		{
			for (auto& command : commandPack.commands)
			{
				History->InjectCommand(std::move(command), commandPack.time, CurrentTime);
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