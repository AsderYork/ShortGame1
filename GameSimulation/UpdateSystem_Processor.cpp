#include "UpdateSystem_Processor.h"
#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\utility.hpp>

namespace GEM::GameSim
{


	bool UpdateSystemProcessor::ApplyCommand(const NetworkCommand * Command, GameTime TimeLag)
	{
		auto CommandRecast = static_cast<const UpdateSystemCommand*>(Command);
		std::shared_ptr<EntityBase> EntIt;
		
		
		EntIt = m_gameSim->m_entities.GetEntity(CommandRecast->m_entityID).lock();
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
					EntIt->GetMixinByID(oldState.first)->ApplyUpdate(ar);
				}

				return false;
			}
		}

		return true;
	}
	uint8_t UpdateSystemProcessor::getIdOfProcessor() const
	{
		return 0;
	}
}