#include "ClientCommandDispatcher.h"
#include "LogHelper.h"

namespace GEM::GameSim
{
	void ClientHistory::InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime)
	{
		m_processors[command->m_header]->ApplyCommand(command.get(), PacketTime);
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

		auto ApplyResult = m_processors[command->m_header]->ApplyCommand(command.get(), 0);
		m_commandBuffer.push_back(CommandStorageItem(std::move(command), ApplyResult));
		return true;
	}
	bool ClientHistory::InsertPerformedCommandInHistory(std::unique_ptr<NetworkCommand>&& command)
	{
		if (m_commandBuffer.full())
		{//Buffer is full! Can't accept this command
			return false;
		}

		//Commands actually recive their ID's in histories only.
		//Actually, server commands doesn't even have to have an ID
		//But it's a serialization problem.
		command->m_uniqueID = m_lastUsedID++;

		m_commandBuffer.push_back(CommandStorageItem(std::move(command)));
		bool Res = m_commandBuffer.full();
		return true;
	}
	void ClientHistory::processHistoryPack(const ServerHistoryPack & serverHistoryPack)
	{
		//Then history might be wrong. But it's also possible, that rejected events 
		//Was also rejected by the client allready, so we should check this first
		for (const auto& rejectedCommand : serverHistoryPack.m_rejectedCommands)
		{
			//Get command, that was rejected
			//Assumes that ID's of commands in a history increments correctly
			auto& rjcCommand = m_commandBuffer[rejectedCommand - m_commandBuffer.front().ptr->m_uniqueID];
			if (rjcCommand.ExpectedToSucceed)
			{
				//If at least one command in history was unexpectedly rejected, history is no longer stable
				m_isHistoryStable = false;
			}
			m_processors[rjcCommand.ptr->m_header]->RejectCommand(rjcCommand.ptr.get());
			rjcCommand.IsRejected = true;
		}
		//After all rejected commands was properly rejected, all the other commands is confirmed up to the number from server
		std::size_t ConfirmedCommandNumber = 0;
		for (auto& ConfirmedCommand : m_commandBuffer)
		{
			//Skip rejected commands
			if (ConfirmedCommand.IsRejected)
			{ 
				ConfirmedCommand.ptr.reset();
				ConfirmedCommandNumber++;
				continue; 
			}

			//If we've reached non-confirmed command, stop
			if (ConfirmedCommand.ptr->m_uniqueID > serverHistoryPack.m_lastRecivedCommand) { break; }

			//And then just confirm commands
			m_processors[ConfirmedCommand.ptr->m_header]->ConfirmCommand(ConfirmedCommand.ptr.get());
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
			m_processors[reverseIt->ptr->m_header]->RollbackCommand(reverseIt->ptr.get());
		}

		for (auto& HistoryCommand : m_commandBuffer)
		{
			HistoryCommand.ExpectedToSucceed = m_processors[HistoryCommand.ptr->m_header]->ReapplyCommand(HistoryCommand.ptr.get());
		}
	}


	void ClientCommandDispatcher::InsertPerformedCommand(std::unique_ptr<NetworkCommand>&& command)
	{		
		auto ptr = command.get();
		if (m_history.InsertPerformedCommandInHistory(std::move(command)))
		{//Insert command only if it's really in history, otherwise pointer will be lost right after this method ends
			m_commandsToSend.emplace_back(ptr);
		}
	}

	void ClientCommandDispatcher::InsertCommand(std::unique_ptr<NetworkCommand>&& command)
	{
		auto ptr = command.get();
		if(m_history.InsertCommandInHistory(std::move(command)))
		{//Insert command only if it's really in history, otherwise pointer will be lost right after this method ends
			m_commandsToSend.emplace_back(ptr);
		}
	}

	void ClientCommandDispatcher::InsertCommandWithoutHistory(std::unique_ptr<NetworkCommand>&& command)
	{
		command->m_uniqueID = 0;
		m_commandsToSend.emplace_back(std::move(command));
	}

	const std::array<NetworkExchangeProcessor*, 256>& ClientCommandDispatcher::getProcessorsTable() const
	{
		return m_history.m_processors;
	}

	void ClientCommandDispatcher::AddProcessor(NetworkExchangeProcessor * newProcessor)
	{
		if (m_history.m_processors[newProcessor->getIdOfProcessor()] == nullptr)
		{
			m_history.m_processors[newProcessor->getIdOfProcessor()] = newProcessor;
		}
		else
		{
			throw std::exception("New processor overwrites current one! Two processors have same ID?");
		}
	}

	void ClientCommandDispatcher::ProcessCommands(std::vector<ServerCommandPack> commandPacks)
	{
		for (auto& commandPack : commandPacks)
		{
			m_history.processHistoryPack(commandPack.HistoryPack);

			for (auto& command : commandPack.commands)
			{
				//Commands from server should be confirmed right away!

				//WARNING! It still unknown if we should Apply and confirm or just confirm
				m_history.m_processors[command->m_header]->ApplyCommand(command.get(), 0);
				m_history.m_processors[command->m_header]->ConfirmCommand(command.get());
			}
			for (auto& processor : m_history.m_processors)
			{
				if (processor == nullptr) { continue; }
				processor->EndNetworkProcessing();
			}
		}
		m_history.ReconsiderHistory();
	}
	ClientCommandPack ClientCommandDispatcher::GatherResults(GameTime CurrentTime)
	{

		ClientCommandPack newCommandPack;
		newCommandPack.time = CurrentTime;
		newCommandPack.commands.swap(m_commandsToSend);		

		return newCommandPack;
	}
}
