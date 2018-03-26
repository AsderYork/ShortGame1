#pragma once
#include "CommandHistoryInterface.h"
#include "SharedDataPackets.h"

#include <boost\circular_buffer.hpp>

#include <vector>
#include <memory>

namespace GEM::GameSim
{
	class ClientHistory : public HistoryInterface
	{
	private:

		boost::circular_buffer<std::pair<std::unique_ptr<NetworkCommand>, bool>> m_commandBuffer;
		NetworkCommandIDType m_lastUsedID;
		bool m_isHistoryStable;

	public:

		ClientHistory(std::size_t buffSize = 400) : m_commandBuffer(buffSize), m_isHistoryStable(true) {}

		virtual void InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime) override;

		/**!
		Commands on client are comming not just from server, that can come from the user itself.
		Commands from user, that require server confirmation, MUST be added into history by this method.
		History will call ApplyCommand() and ony other required methods to this commands as it sees fit.

		\returns Returns true if command was added successfully, otherwise false is returned

		\note It is recomended to disconnect if command wasn't added, becouse it might lead to desynchronyzation.
		*/
		bool InsertCommandInHistory(std::unique_ptr<NetworkCommand>&& command);


		void processHistoryPack(const ServerHistoryPack& serverHistoryPack);

		void ReconsiderHistory();

	};


	class ClientCommandProcessor
	{
		std::vector<std::unique_ptr<NetworkCommand>> m_commandsToSend;
		ClientHistory m_history;

	public:

		void ProcessCommands(std::vector<ServerCommandPack> commandPacks);

		ClientCommandPack GatherResults(GameTime CurrentTime);
	};
}