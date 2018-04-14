#include "SharedDataPackets.h"

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
			std::visit([&](auto&& arg) {

			}, command);


		}
	}
}