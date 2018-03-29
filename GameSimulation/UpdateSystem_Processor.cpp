#include "UpdateSystem_Processor.h"
#include "UpdateSystem_Command.h"
#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\utility.hpp>

namespace GEM::GameSim
{


	void UpdateSystemProcessor::RollbackCommand(const NetworkCommand * Command)
	{
	}

	bool UpdateSystemProcessor::ReapplyCommand(const NetworkCommand * Command)
	{
		return false;
	}

	void UpdateSystemProcessor::ConfirmCommand(const NetworkCommand * Command)
	{
	}

	void UpdateSystemProcessor::RejectCommand(const NetworkCommand * Command)
	{
	}

	void UpdateSystemProcessor::SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command)
	{
		auto CommandRecast = static_cast<const UpdateSystemCommand*>(Command);
		ar((*CommandRecast));
	}

	std::unique_ptr<NetworkCommand> GEM::GameSim::UpdateSystemProcessor::deserializeCommand(cereal::BinaryInputArchive & ar)
	{
		auto NewCommand = std::make_unique<UpdateSystemCommand>();
		ar((*NewCommand));
		return NewCommand;
	}

	bool UpdateSystemProcessor::ApplyCommand(const NetworkCommand * Command, GameTime PacketTime)
	{
		auto CommandRecast = static_cast<const UpdateSystemCommand*>(Command);
		EntityBase* EntIt=nullptr;

		if (!CommandRecast->m_mixins.empty())
		{
			EntIt = m_gameSim->AddEntity(CommandRecast->m_entityID, CommandRecast->m_mixins);
		}
		else
		{
			EntIt = m_gameSim->m_entities.GetEntity(CommandRecast->m_entityID);
		}

		bool UpdateFullyAccepted = true;
		for (auto& mixin : CommandRecast->m_perMixinUpdates)
		{
			std::stringstream sstream(mixin.second);
			cereal::BinaryInputArchive ar(sstream);
			EntIt->GetMixinByID(mixin.first)->ApplyEvent(ar);
		}

		return true;
	}
	uint8_t UpdateSystemProcessor::getIdOfProcessor() const
	{
		return uint8_t();
	}
}