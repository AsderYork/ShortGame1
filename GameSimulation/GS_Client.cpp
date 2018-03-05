#include "GS_Client.h"
#include "UpdateStructures.h"

#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\chrono.hpp>

namespace GEM::GameSim
{
	bool GS_Client::Tick(float Delta, cereal::BinaryInputArchive& archive, std::stringstream& OutputStream, bool ArchiveIsEmpty)
	{
		if (!ArchiveIsEmpty)
		{
			OutOfSync_Packet GameHistoryOOSPacket;
			InSync_Packet GameHistoryInSyncPacket;
			std::vector<UpdateData> Updates;
			std::chrono::system_clock::time_point UpdateTime;
			try
			{
				while (true)
				{
					std::vector<UpdateData> NextUpdates;
					OutOfSync_Packet tmpOOSPackets;
					InSync_Packet tmpInSyncPackets;

					archive(UpdateTime, tmpOOSPackets, tmpInSyncPackets, NextUpdates);

					GameHistoryOOSPacket.Add(tmpOOSPackets);
					GameHistoryInSyncPacket.Add(tmpInSyncPackets);

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
					//return 0;
				}
			}
			m_lastUpdateTime = UpdateTime;

			m_gameHistory.ProcessOutOfSync(GameHistoryOOSPacket);
			m_gameHistory.ProcessInSync(GameHistoryInSyncPacket);

			for (auto& update : Updates)
			{
				static bool skipPlayerMovement = false;
				/*Player character is controlled by a clinet and it prediction mechanisms is used
				to eliminate latency. So if an update is addressed to a player entity, we may prefer to
				not apply the update in expectation that we allready have a newer version of gamestate.
				*/
				if ((update.EntityID == m_playerCharacterID) && (m_gameHistory.IsHistoryAccurate()))
				{//If history is accurate, skip update for a player position. Right now this is the only mixin, that supports this feature
					skipPlayerMovement = true;
				}

				if (std::holds_alternative<EntityAppearingUpdate>(update.Data))
				{
					m_gs.AddEntity(update.EntityID, std::get<EntityAppearingUpdate>(update.Data).EntityMixins);
					auto& MixinUpdate = std::get<EntityAppearingUpdate>(update.Data).PerMixinUpdates;

					

					auto ent = m_gs.m_entities.GetEntity(update.EntityID);
					for (auto& mixin : MixinUpdate)
					{
						std::stringstream sstream(mixin.second);
						cereal::BinaryInputArchive ar(sstream);
						if (skipPlayerMovement && (mixin.first == Mixin_Movable::MixinID)) { continue; }
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
						if (skipPlayerMovement && (mixin.first == Mixin_Movable::MixinID)) { continue; }
						ent->GetMixinByID(mixin.first)->ReciveUpdate(ar);
					}
				}



			}
		}

		auto ent = m_gs.m_entities.GetEntity(m_playerCharacterID);
		/**!
		If player have not yet been set, that means that simulation is not initialized enough for all that history
		manipulation stuff. So we'll just perform one tick of simulation and skip everything else
		*/
		if (ent == nullptr) { return m_gs.Tick(Delta); }


		if (!m_gameHistory.IsHistoryAccurate())
		{//If history is inaccurate, re-apply all the events!
		 //For now only PlayerCharacter is controlled by the history
			auto ApplyUpdateLambda = [&](GameHistoryController::StateChange* change)
			{
				cereal::BinaryInputArchive ar(change->data);
				//Only MixinMovable is affected by the history, so we just send all updates to it.
				ent->GetMixinByID(Mixin_Movable::MixinID)->ReciveUpdate(ar);
				//For now we just assume, that any update is correct
				return true;
			};
			m_gameHistory.ReApplyUpdates(ApplyUpdateLambda);
		}

		//Perform basic simulation tick
		auto Retval = m_gs.Tick(Delta);

		//Gather new PlayerUpdate for history.
		GameHistoryController::StateChange newUpdate;
		cereal::BinaryOutputArchive UpdateAr(newUpdate.data);
		ent->GetMixinByID(Mixin_Movable::MixinID)->SendUpdate(UpdateAr, Mixin_base::UpdateReason::REGULAR);
		auto UpdateForSending = m_gameHistory.AddNewUpdate(newUpdate);

		//Send new events to a server
		cereal::BinaryOutputArchive OutputAr(OutputStream);
		OutputAr(UpdateForSending);

		return Retval;
	}

}