#pragma once
#include "GameTime.h"
#include "NetworkCommandBase.h"
#include "Helper_VariableSizeSerialization.h"
#include <memory>
#include <vector>

namespace GEM::GameSim
{
	struct ServerHistoryPack
	{
		uint64_t m_lastRecivedCommand;
		std::vector<uint64_t> rejectedCommands;

		inline ServerHistoryPack() : m_lastRecivedCommand(0) {};

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(m_lastRecivedCommand);
			//To much commands in one pack!
			assert(commands.size() < std::numeric_limits<uint16_t>::max());
			Helper::SaveVector<uint16_t>(archive, m_lastRecivedCommand);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(m_lastRecivedCommand);
			Helper::LoadVector<uint16_t>(archive, m_lastRecivedCommand);
		}
	};

	struct ServerCommandPack
	{
		GameTime time;
		std::vector<std::unique_ptr<NetworkCommand>> commands;
		ServerHistoryPack HistoryPack;

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(time);
			archive(HistoryPack);
			//To much commands in one pack!
			assert(commands.size() < std::numeric_limits<uint32_t>::max());
			Helper::SaveVector<uint32_t>(archive, commands);


		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(time);
			archive(HistoryPack);
			Helper::LoadVector<uint32_t>(archive, commands);
		}
	};

	struct ClientCommandPack
	{
		GameTime time;
		std::vector<std::unique_ptr<NetworkCommand>> commands;

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(time);
			archive(HistoryPack);
			//To much commands in one pack!
			assert(commands.size() < std::numeric_limits<uint32_t>::max());
			Helper::SaveVector<uint32_t>(archive, commands);


		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(time);
			archive(HistoryPack);
			Helper::LoadVector<uint32_t>(archive, commands);
		}
	};
}