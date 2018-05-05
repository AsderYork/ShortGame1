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
#include <variant>

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

		void SerealizeIn(cereal::BinaryOutputArchive& ar, const std::array<NetworkExchangeProcessor*, 256>& Processors) const;

		void SerializeOut(cereal::BinaryInputArchive& ar, const std::array<NetworkExchangeProcessor*, 256>& Processors);
	};

	struct ClientCommandPack_Server
	{
		GameTime time;
		std::vector<std::unique_ptr<NetworkCommand>> commands;

		/**!
		Serializes CommandPack from binary string.
		\param[in] ar InputArchive of binary string, that contains command pack
		\param[in] Processors processors that provide serilaization facilities for commands
		\returns Returns the ammount of bytes, that where read
		*/
		void SerializeOut(cereal::BinaryInputArchive& ar, const std::array<NetworkExchangeProcessor*, 256>& Processors);

	};

	struct ClientCommandPack
	{
		GameTime time;
		//Command queued for sending can either completely be owned by Pack
		//Or it can be owned by some other systems and just refrence it
		//On reciving end though, it's allways unqiue_ptr
		std::vector<std::variant<std::unique_ptr<NetworkCommand>, NetworkCommand*>> commands;

		void SerealizeIn(cereal::BinaryOutputArchive& ar, const std::array<NetworkExchangeProcessor*, 256>& Processors) const;
	};
}