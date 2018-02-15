#include "GS_Client.h"
#include "UpdateStructures.h"

#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\chrono.hpp>

namespace GEM::GameSim
{
	bool GS_Client::Tick(float Delta, cereal::BinaryInputArchive& archive, bool ArchiveIsEmpty)
	{
		if (!ArchiveIsEmpty)
		{
			std::vector<UpdateData> Updates;
			std::chrono::system_clock::time_point UpdateTime;
			try
			{
				while (true)
				{
					std::vector<UpdateData> NextUpdates;
					archive(UpdateTime, NextUpdates);
					Updates.insert(Updates.end(), NextUpdates.begin(), NextUpdates.end());
				}
			}
			catch (...)
			{}

			if (!m_timeIsSet)//Check if it's the first update of session
			{//It is				
				m_timeIsSet = true;
			}
			else
			{
				if (std::chrono::duration_cast<std::chrono::milliseconds>(UpdateTime - m_lastUpdateTime).count() > 1000)
				{//Client can't keep up with the server updates!
					return 0;
				}
			}
			m_lastUpdateTime = UpdateTime;

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
		}


		//Perform basic simulation tick
		auto Retval = m_gs.Tick(Delta);
		return Retval;
	}
}