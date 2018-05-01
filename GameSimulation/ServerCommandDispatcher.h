#pragma once
#include "Player_id_type.h"
#include "CommandHistoryInterface.h"
#include "SharedDataPackets.h"

#include <vector>
#include <array>

namespace GEM::GameSim
{
	class ServerCommandDispatcher;

	class ServerHistory
	{
	private:
		PLAYER_ID_TYPE m_playerID;
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

		/**!
		Inserts command in processing loop for a given client
		\param[in] command A command, that should be processed
		\param[in] PacketTime A time from packet, that came with a given command
		\param[in] CurrTime Current time of a server simulation
		*/
		virtual void InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime, GameTime CurrTime);

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

		ServerHistory(ServerCommandDispatcher& dispatcher, PLAYER_ID_TYPE playerID);
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

		/**!
		Process all the commands in commandPack for a client with a given history
		*/
		void ProcessCommands(std::vector<ClientCommandPack_Server> commandPacks, ServerHistory* History, GameTime CurrentTime);
		

		ServerCommandPack GatherResults(ServerHistory* History, GameTime CurrentTime);

		const std::array<NetworkExchangeProcessor*, 256>& getProcessorsTable() const { return m_processors; }
	};

}