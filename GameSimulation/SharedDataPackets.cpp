#include "SharedDataPackets.h"
#include "LogHelper.h"

namespace GEM::GameSim
{
	void ClientCommandPack::SerealizeIn(cereal::BinaryOutputArchive & ar, const std::array<NetworkExchangeProcessor*, 256>& Processors) const
	{
		ar(time);

		ar(static_cast<uint32_t>(commands.size()));
		for (auto& command : commands)
		{
			if (std::holds_alternative<NetworkCommand*>(command))
			{
				auto& ptr = std::get<NetworkCommand*>(command);
				ar(ptr->m_header);
				ar(ptr->m_uniqueID);
				Processors[ptr->m_header]->SerializeCommand(ar, ptr);
			}
			else if (std::holds_alternative<std::unique_ptr<NetworkCommand>>(command))
			{
				auto& ptr = std::get<std::unique_ptr<NetworkCommand>>(command);
				ar(ptr->m_header);
				ar(ptr->m_uniqueID);
				Processors[ptr->m_header]->SerializeCommand(ar, ptr.get());
			}
			


		}
	}
	
	void ServerCommandPack::SerealizeIn(cereal::BinaryOutputArchive & ar, const std::array<NetworkExchangeProcessor*, 256>& Processors) const
	{
		ar(time);
		ar(HistoryPack);

		uint32_t CastSize = static_cast<uint32_t>(commands.size());
		ar(CastSize);
		for (const auto& command : commands)
		{
			ar(command->m_header);
			//ar(command->m_uniqueID); Commands from server doesn't require confirmation hence they don't need unique ID
			Processors[command->m_header]->SerializeCommand(ar, command.get());
		}
	}
	void ServerCommandPack::SerializeOut(cereal::BinaryInputArchive & ar, const std::array<NetworkExchangeProcessor*, 256>& Processors)

	{
		ar(time);
		ar(HistoryPack);
		uint32_t commandsSize = 0;
		ar(commandsSize);

		uint8_t header;

		for (uint32_t i = 0; i < commandsSize; i++)
		{
			ar(header);
			if (Processors[header] == nullptr) {
				LOGCATEGORY("ServerCommandPack/SerializeOut").error("Command is addressed to processor %i that doesn't exist!", header);
				continue;
			}
			commands.emplace_back(Processors[header]->deserializeCommand(ar));
			commands.back()->m_header = header;
		}
	}
}