#pragma once
#include "CommandHistoryInterface.h"
#include "SharedDataPackets.h"
#include <vector>

namespace GEM::GameSim
{
	class ServerHistory : public HistoryInterface
	{
	public:
		uint64_t m_lastRecivedCommand;
		std::vector<uint64_t> m_rejectedCommands;


		virtual void InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime) override {
			m_lastRecivedCommand = command->m_uniqueID;

			if (!m_processors[command->m_header]->ApplyCommand(command.get()))
			{
				m_rejectedCommands.push_back(m_lastRecivedCommand);
			}
		}

		ServerHistoryPack getHistoryPack()
		{
			ServerHistoryPack result;
			result.m_lastRecivedCommand = m_lastRecivedCommand;
			result.rejectedCommands.swap(m_rejectedCommands);
			return result;
		}
	};

	class ServerCommandDispatcher
	{
		std::vector<std::unique_ptr<NetworkCommand>> m_commandsToSend;

	public:	

		void ProcessCommands(std::vector<ClientCommandPack> commandPacks, ServerHistory* History);
		

		ServerCommandPack GatherResults(ServerHistory* History, GameTime CurrentTime);

	};
}