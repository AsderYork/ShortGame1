#include "ClientCommandDispatcher.h"

namespace GEM::GameSim
{
	void ClientHistory::InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime)
	{
		m_processors[command->m_header]->ApplyCommand(command.get());
	}
	bool ClientHistory::InsertCommandInHistory(std::unique_ptr<NetworkCommand>&& command)
	{
		if (m_commandBuffer.full())
		{//Buffer is full! Can't accept this command
			return false;
		}

		//Commands actually recive their ID's in histories only.
		//Actually, server commands doesn't even have to have an ID
		//But it's a serialization problem.
		command->m_uniqueID = m_lastUsedID++;

		auto ApplyResult = m_processors[command->m_header]->ApplyCommand(command.get());
		m_commandBuffer.push_back(std::make_pair(std::move(command), ApplyResult));
		return true;
	}
	void ClientHistory::processHistoryPack(const ServerHistoryPack & serverHistoryPack)
	{
		//Then history might be wrong. But it's also possible, that rejected events 
		//Was also rejected by the client allready, so we should check this first
		for (const auto& rejectedCommand : serverHistoryPack.rejectedCommands)
		{
			//Get command, that was rejected
			//Assumes that ID's of commands in a history increments correctly
			auto& rjcCommand = m_commandBuffer[rejectedCommand - m_commandBuffer.begin()->first->m_uniqueID];
			if (rjcCommand.second)
			{
				//If at least one command in history was unexpectedly rejected, history is no longer stable
				m_isHistoryStable = false;
			}
			m_processors[rjcCommand.first->m_header]->RejectCommand(rjcCommand.first.get());
			rjcCommand.first.reset();
		}
		//After all rejected commands was properly rejected, all the other commands is confirmed up to the number from server
		std::size_t ConfirmedCommandNumber = 0;
		for (auto& ConfirmedCommand : m_commandBuffer)
		{
			//Skip rejected commands
			if (ConfirmedCommand.first == nullptr) { continue; }

			//If we've reached non-confirmed command, stop
			if (ConfirmedCommand.first->m_uniqueID > serverHistoryPack.m_lastRecivedCommand) { break; }

			//And then just confirm commands
			m_processors[ConfirmedCommand.first->m_header]->ConfirmCommand(ConfirmedCommand.first.get());
			ConfirmedCommandNumber++;
		}
		//Erase confirmed commands. And rejected ones. It doesn't matter no more!
		m_commandBuffer.erase_begin(ConfirmedCommandNumber);

	}
	void ClientHistory::ReconsiderHistory()
	{
		//History is stable, no reconsidiration is needed
		if (m_isHistoryStable) { return; }

		for (auto reverseIt = m_commandBuffer.rbegin(); reverseIt != m_commandBuffer.rend(); reverseIt++)
		{
			m_processors[reverseIt->first->m_header]->RollbackCommand(reverseIt->first.get());
		}

		for (auto& HistoryCommand : m_commandBuffer)
		{
			HistoryCommand.second = m_processors[HistoryCommand.first->m_header]->ReapplyCommand(HistoryCommand.first.get());
		}
	}
	void ClientCommandProcessor::ProcessCommands(std::vector<ServerCommandPack> commandPacks)
	{
		for (auto& commandPack : commandPacks)
		{
			m_history.processHistoryPack(commandPack.HistoryPack);

			for (auto& command : commandPack.commands)
			{
				m_history.InjectCommand(std::move(command), commandPack.time);
			}
		}
		m_history.ReconsiderHistory();
	}
	ClientCommandPack ClientCommandProcessor::GatherResults(GameTime CurrentTime)
	{
		ClientCommandPack newCommandPack;
		newCommandPack.time = CurrentTime;
		newCommandPack.commands.swap(m_commandsToSend);
		return newCommandPack;
	}
}
