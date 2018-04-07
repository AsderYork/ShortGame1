#pragma once
#include "GameTime.h"
#include "NetworkCommandBase.h"
#include "NetworkCommandsProcessor.h"
#include "Helper_VariableSizeSerialization.h"
#include <memory>
#include <vector>
#include <array>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cassert>

namespace GEM::GameSim
{
	struct ServerHistoryPack
	{
		uint64_t m_lastRecivedCommand;
		std::vector<uint64_t> m_rejectedCommands;

		inline ServerHistoryPack() : m_lastRecivedCommand(0) {};

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(m_lastRecivedCommand);
			Helper::SaveVector<uint16_t>(archive, m_rejectedCommands);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(m_lastRecivedCommand);
			Helper::LoadVector<uint16_t>(archive, m_rejectedCommands);
		}
	};

	struct ServerCommandPack
	{
		GameTime time;
		ServerHistoryPack HistoryPack;
		std::vector<std::unique_ptr<NetworkCommand>> commands;

		void SerealizeIn(cereal::BinaryOutputArchive& ar,const std::array<NetworkExchangeProcessor*, 256>& Processors) const
		{
			ar(time);
			ar(HistoryPack);

			ar( static_cast<uint32_t>(commands.size()) );
			for (const auto& command : commands)
			{
				ar(command->m_header);
				//ar(command->m_uniqueID); Commands from server doesn't require confirmation hence they don't need unique ID
				Processors[command->m_header]->SerializeCommand(ar, command.get());
			}
		}

		void SerializeOut(cereal::BinaryInputArchive& ar,const std::array<NetworkExchangeProcessor*, 256>& Processors)
		{
			ar(time);
			ar(HistoryPack);
			uint32_t commandsSize = 0;
			ar(commandsSize);

			uint8_t header;

			for (uint32_t i = 0; i < commandsSize; i++)
			{
				ar(header);
				commands.emplace_back(Processors[header]->deserializeCommand(ar));
				commands.back()->m_header = header;
			}
		}
	};

	struct ClientCommandPack
	{
		GameTime time;
		std::vector<std::unique_ptr<NetworkCommand>> commands;

		void SerealizeIn(cereal::BinaryOutputArchive& ar, const std::array<NetworkExchangeProcessor*, 256>& Processors) const
		{
			ar(time);

			ar(static_cast<uint32_t>(commands.size()));
			for (const auto& command : commands)
			{
				ar(command->m_header);
				ar(command->m_uniqueID);
				Processors[command->m_header]->SerializeCommand(ar, command.get());
			}
		}

		void SerializeOut(cereal::BinaryInputArchive& ar, const std::array<NetworkExchangeProcessor*, 256>& Processors)
		{
			ar(time);
			uint32_t commandsSize = 0;
			ar(commandsSize);

			uint8_t header;
			NetworkCommandIDType id;

			for (uint32_t i = 0; i < commandsSize; i++)
			{
				ar(header);
				ar(id);
				commands.emplace_back(Processors[header]->deserializeCommand(ar));
				commands.back()->m_header = header;
			}
		}
	};
}