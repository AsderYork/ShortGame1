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

	bool UpdateSystemProcessor::ApplyCommand(const NetworkCommand * Command, GameTime TimeLag)
	{
		auto CommandRecast = static_cast<const UpdateSystemCommand*>(Command);
		EntityBase* EntIt=nullptr;
		
		
		EntIt = m_gameSim->m_entities.GetEntity(CommandRecast->m_entityID);
		if (EntIt == nullptr) { return false; }

		bool UpdateFullyAccepted = true;

		//Store current mixin states in case of rollback
		std::vector<std::pair<MIXIN_ID_TYPE, std::stringstream>> PrevStates;

		for (auto& mixin : CommandRecast->m_perMixinUpdates)
		{
			{//Current state storing part
				PrevStates.emplace_back();
				PrevStates.back().first = mixin.first;
				cereal::BinaryOutputArchive storeAr(PrevStates.back().second);
				EntIt->GetMixinByID(mixin.first)->SendUpdate(storeAr, Mixin_base::UpdateReason::APPEAR);
			}

			
			std::stringstream sstream(mixin.second);
			cereal::BinaryInputArchive ar(sstream);
			if (!EntIt->GetMixinByID(mixin.first)->CheckAndReciveUpdate(ar, TimeLag))
			{
				//If at least one update failed. Rollback and return false!
				for (auto& oldState : PrevStates)
				{
					cereal::BinaryInputArchive ar(oldState.second);
					EntIt->GetMixinByID(oldState.first)->ApplyEvent(ar);
				}

				return false;
			}
		}

		return true;
	}
	uint8_t UpdateSystemProcessor::getIdOfProcessor() const
	{
		return uint8_t();
	}
}