#pragma once
#include "CommandHistoryInterface.h"
#include "SharedDataPackets.h"

#include <boost\circular_buffer.hpp>

#include <vector>
#include <memory>
#include <array>

namespace GEM::GameSim
{
	class ClientCommandDispatcher;

	class ClientHistory : public HistoryInterface
	{
	private:
		friend class ClientCommandDispatcher;
		/**!
		Every processor have unique ID and we have less then 256 processors.
		So we just use this array as table for quick search of processors
		*/
		std::array<NetworkExchangeProcessor*, 256> m_processors;

		struct CommandStorageItem
		{
			//A command itself.
			std::unique_ptr<NetworkCommand> ptr;
			//Command may fail during re-applyment. If it is allready expected, that this command fails, and then server rejects it
			//It actually doesn't change the state of a client. This variable allows tracking of such things
			bool ExpectedToSucceed;
			//When command get's rejected, we can't immediately remove it from buffer
			//Becouse we still need it's ID. So we just remember that it must be freed instead of re-applied
			//And do that during re-applyment state
			bool IsRejected;

			CommandStorageItem(std::unique_ptr<NetworkCommand>&& _ptr, bool ExpectedResult=true) :
				ptr(std::move(_ptr)),
				ExpectedToSucceed(ExpectedResult),
				IsRejected(false)
			{}
		};

		boost::circular_buffer<CommandStorageItem> m_commandBuffer;
		NetworkCommandIDType m_lastUsedID;
		bool m_isHistoryStable;

	public:

		ClientHistory(std::size_t buffSize = 400) : m_commandBuffer(buffSize), m_isHistoryStable(true), m_lastUsedID(1) { m_processors.fill(nullptr); }

		virtual void InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime) override;

		/**!
		Commands on client are comming not just from server, they can come from the user itself.
		Commands from user, that require server confirmation, MUST be added into history by this method.
		History will call ApplyCommand() and ony other required methods to this commands as it sees fit.

		\returns Returns true if command was added successfully, otherwise false is returned

		\note It is recomended to disconnect if command wasn't added, becouse it might lead to desynchronyzation.
		*/
		bool InsertCommandInHistory(std::unique_ptr<NetworkCommand>&& command);

		/**!
		Works just like \c InsertCommandInHistory
		but assumes that command is allready performed and skips initial call to \c ApplyCommand
		*/
		bool InsertPerformedCommandInHistory(std::unique_ptr<NetworkCommand>&& command);

		void processHistoryPack(const ServerHistoryPack& serverHistoryPack);

		void ReconsiderHistory();

	};


	class ClientCommandDispatcher
	{
		std::vector<std::variant<std::unique_ptr<NetworkCommand>, NetworkCommand*>> m_commandsToSend;

	public:

		ClientHistory m_history;


		/**!
		Prepares a command to be sent without perorming it and inserts it into history		
		*/
		void InsertPerformedCommand(std::unique_ptr<NetworkCommand>&& command);

		/**!
		Inserts command in a history and prepares it to be sent out.
		*/
		void InsertCommand(std::unique_ptr<NetworkCommand>&& command);

		void InsertCommandWithoutHistory(std::unique_ptr<NetworkCommand>&& command);

		const std::array<NetworkExchangeProcessor*, 256>& getProcessorsTable() const;

		void AddProcessor(NetworkExchangeProcessor* newProcessor);

		void ProcessCommands(std::vector<ServerCommandPack> commandPacks);

		ClientCommandPack GatherResults(GameTime CurrentTime);
	};
}