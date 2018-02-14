#include "GS_Client.h"
#include "UpdateStructures.h"

#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>
#include <cereal\types\string.hpp>

namespace GEM::GameSim
{
	bool GS_Client::Tick(float Delta, cereal::BinaryInputArchive& archive)
	{
		std::vector<UpdateData> Updates;
		archive(Updates);

		for (auto& update : Updates)
		{
			if (std::holds_alternative<EntityAppearingUpdate>(update.Data))
			{
				m_gs.AddEntity(update.EntityID, std::get<EntityAppearingUpdate>(update.Data).EntityMixins);
				auto& MixinUpdate = std::get<EntityAppearingUpdate>(update.Data).PerMixinUpdates;

				auto ent = m_gs.m_entities.GetEntity(update.EntityID);
				for (auto& mixin : MixinUpdate)
				{
					std::stringstream sstream(mixin.second);
					cereal::BinaryInputArchive ar(sstream);
					ent->GetMixinByID(mixin.first)->ReciveUpdate(ar);
				}

			}
			else
			{
				auto& MixinUpdate = std::get<EntityRegularUpdate>(update.Data).PerMixinUpdates;
				auto ent = m_gs.m_entities.GetEntity(update.EntityID);
				for (auto& mixin : MixinUpdate)
				{
					std::stringstream sstream(mixin.second);
					cereal::BinaryInputArchive ar(sstream);
					ent->GetMixinByID(mixin.first)->ReciveUpdate(ar);
				}
			}

			
			
		}

		//Perform basic simulation tick
		auto Retval = m_gs.Tick(Delta);
		
		
		return Retval;
	}
}