#pragma once
#include "CommandHistoryInterface.h"
#include "SharedDataPackets.h"
#include <vector>
#include <array>

namespace GEM::GameSim
{
	class ServerCommandDispatcher;

	class ServerHistory : public HistoryInterface
	{
	public:

		/**!
		Every processor have unique ID and we have less then 256 processors.
		So we just use this array as table for quick search of processors
		*/
		std::array<NetworkExchangeProcessor*, 256>& m_processors;
		uint64_t m_lastRecivedCommand;
		std::vector<uint64_t> m_rejectedCommands;

		//Commands to send are on per-player basis, so they are kept as history of network exchange with a palyer. I mean, it's still history, right?
		std::vector<std::unique_ptr<NetworkCommand>> m_commandsToSend;


		virtual void InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime) override;

		/**!
		Server usualy performs command as part of it's simulation and then just sends updates.
		That means that at the time when it's time to send something, a command is allready performed on the server,
		so all you need is to just make it so that it will be sent
		\param[in] command A command, that shoud be sent
		*/
		void SendAllreadyPerformedCommand(std::unique_ptr<NetworkCommand>&& command)
		{

			m_commandsToSend.emplace_back(std::move(command));
		}

		ServerHistoryPack getHistoryPack()
		{
			ServerHistoryPack result;
			result.m_lastRecivedCommand = m_lastRecivedCommand;
			result.m_rejectedCommands.swap(m_rejectedCommands);
			return result;
		}

		ServerHistory(ServerCommandDispatcher& dispatcher);
	};


	class ServerCommandDispatcher
	{
		friend class ServerHistory;
		std::array<NetworkExchangeProcessor*, 256> m_processors;

	public:

		void AddProcessor(NetworkExchangeProcessor* newProcessor)
		{
			if (m_processors[newProcessor->getIdOfProcessor()] == nullptr)
			{
				m_processors[newProcessor->getIdOfProcessor()] = newProcessor;
			}
			else
			{
				throw std::exception("New processor overwrites current one! Two processors have same ID?");
			}
		}

		void ProcessCommands(std::vector<ClientCommandPack> commandPacks, ServerHistory* History);
		

		ServerCommandPack GatherResults(ServerHistory* History, GameTime CurrentTime);

		const std::array<NetworkExchangeProcessor*, 256>& getProcessorsTable() const { return m_processors; }
	};

}