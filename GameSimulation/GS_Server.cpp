#include "GS_Server.h"
#include <sstream>
#include <limits>


namespace GEM::GameSim
{

	
	

	GS_Server::PlayerUpdatePack GS_Server::GatherDataForPlayer(PLAYER_ID_TYPE id)
	{
		PlayerUpdatePack PPI;

		auto PlayerInfo = m_perPlayerInfo.find(id);

		PPI.UpdateTime = m_gs.getGameTime();
		PPI.updates.swap(PlayerInfo->second.UpdateVector);
		PPI.OOS.DesyncronizedEvents.swap(PlayerInfo->second.currentOOS.DesyncronizedEvents);
		PPI.InSync.LastSynced = PlayerInfo->second.currentInSync.LastSynced;

		return PPI;
	}
	void GS_Server::ReciveSynchroUpdatesFromClient(PLAYER_ID_TYPE id, cereal::BinaryInputArchive ar)
	{
		auto& TmpPlayerInfo = m_perPlayerInfo.find(id)->second;

		/*There is no obvious way to check, how many packets server have sent us
		So we have to realy on on exception for a main flow controll.
		*/
		try
		{
			while (true)
			{
				SyncingUpdate_Packet tmpStateHolder;
				GameTime tmpTime;
				ar(tmpTime, tmpStateHolder);
				TmpPlayerInfo.SynchroUpdates.emplace_back(tmpStateHolder, tmpTime);
			}


		}
		catch (cereal::Exception&) {}
	}
	

	
	std::pair<EntityRegularUpdate, EntityAppearingUpdate> GS_Server::GetEntityUpdate(std::pair<ENTITY_ID_TYPE, EntityBase*>& Entity)
	{
		auto& mixinvec = Entity.second->getAllMixins();

		EntityRegularUpdate Regular;
		EntityAppearingUpdate Appearing;

		for (auto& mixin : mixinvec)
		{
			std::stringstream DataStream;
			if (mixin->NeedsUpdate())
			{
				{
					cereal::BinaryOutputArchive ar(DataStream);
					mixin->SendUpdate(ar, Mixin_base::UpdateReason::REGULAR);
				}
				Regular.PerMixinUpdates.emplace_back(mixin->getMixinID(), DataStream.str());
			}

			DataStream.str(std::string());
			{
				cereal::BinaryOutputArchive ar(DataStream);
				mixin->SendUpdate(ar, Mixin_base::UpdateReason::APPEAR);
			}
			Appearing.EntityMixins.push_back(mixin->getMixinID());
			Appearing.PerMixinUpdates.emplace_back(mixin->getMixinID(), DataStream.str());

		}


		return std::make_pair(Regular, Appearing);
	}

	void GS_Server::ProcessPlayerSyncingUpdates()
	{
		for (auto& player : m_gs.m_players.getPlayersVector())
		{
			auto& playerIt = m_perPlayerInfo.find(player.id);
			if (playerIt->second.SynchroUpdates.size() == 0) {continue; }

			for (auto& update : playerIt->second.SynchroUpdates)
			{
				//Check the update.
					//if update doesn't fit the rules of the simulation, add ID to OOS and skip

				//If update fits, apply it
				cereal::BinaryInputArchive ar(update.first.UpdateData.data);
				//Lag get's devided by 2 in assumption, that it takes the same amount of time for a packet to go from server to client
				//And from clinet to server
				player.characterPtr->GetMixinByID(Mixin_Movable::MixinID)->ReciveUpdate(ar, (m_gs.getGameTime()-update.second)/2);
			}
			playerIt->second.currentInSync.LastSynced = playerIt->second.SynchroUpdates.back().first.UniuqeEventID;
			playerIt->second.SynchroUpdates.clear();
			
		}

		
	}

	std::optional<PlayerTicket> GS_Server::NewPlayerRoutine(Player && player)
	{
		auto newPlayer = m_gs.m_players.addPlayer(player);
		if (!newPlayer) { return std::nullopt; }
		
		auto [CharPtr, CharId] = m_gs.AddEntity({ Mixin_Movable::MixinID, Mixin_Health::MixinID});
		newPlayer->get().characterPtr = CharPtr;
		newPlayer->get().characterID = CharId;

		m_perPlayerInfo.emplace(newPlayer->get().id, PerPlayerInfo());


		return newPlayer;
	}

	void GS_Server::RemovePlayer(PlayerTicket && player)
	{
		m_perPlayerInfo.erase(player.get().id);
		m_gs.m_entities.RemoveEntity(player.get().characterID);
		m_gs.m_players.RemovePlayer(std::move(player));
	}

	void  GS_Server::Tick(float Delta)
	{

		ProcessPlayerSyncingUpdates();
		//Perform basic tick
		auto Reval = m_gs.Tick(Delta);

		auto& Players = m_gs.m_players.getPlayersVector();
		//Gather all the changes.
		std::unique_ptr<EntityController::EntityListIterator> iter = nullptr;

		auto Entity = m_gs.m_entities.IterateOverEntities(std::move(iter));
		/*But we not just sending updates. It is possible, that client completely miss an entity!
		And we deside it on a per-client basis.
		Players that just connected to a server should recive all neccessary entities,
		Some entities might leave the view of some players, so for them, we should not retranslate all the changes.

		*/

		bool IsThereGeneralUpdate;

		while (Entity)
		{
			auto[RegUpdate, AppearUpdate] = GetEntityUpdate(Entity.value());

			UpdateData RegEntry(std::move(RegUpdate), Entity->first);
			UpdateData AppearingEntry(std::move(AppearUpdate), Entity->first);
			
			
			if (RegUpdate.PerMixinUpdates.size() == 0)
			{//This entity doesn't have a regular update for this tick.
				IsThereGeneralUpdate = false;
			}
			else
			{
				IsThereGeneralUpdate = true;
			}

			auto EntityMovability = static_cast<Mixin_Movable*>(Entity->second->GetMixinByID(Mixin_Movable::MixinID));


			if (EntityMovability != nullptr)//Check if its a movable entity
			{//It is.
				for (auto& pl : Players)
				{
					auto PlayerPos = static_cast<Mixin_Movable*>(pl.characterPtr->GetMixinByID(Mixin_Movable::MixinID))->getPos();

					//Check if player have this entity
					if (pl.trackedEntities.find(Entity->first) != pl.trackedEntities.end())
					{//It have the entity
						//Check if it's in a radius for update
						if (PlayerPos.distance(EntityMovability->getPos()) < m_playerUpldateRadius)
						{//It is in radius of update. Send regular
							if (IsThereGeneralUpdate)
							{
								m_perPlayerInfo.find(pl.id)->second.UpdateVector.emplace_back(RegEntry);
							}
						}
						else
						{//It's out of radius. Player looses this entity.
							pl.trackedEntities.erase(Entity->first);
						}
					}
					else
					{//Player does not have this entity
						//Check if it's in radius for update
						if (PlayerPos.distance(EntityMovability->getPos()) < m_playerUpldateRadius)
						{//It is in radius of update. Send Appearing
							pl.trackedEntities.insert(Entity->first);
							m_perPlayerInfo.find(pl.id)->second.UpdateVector.emplace_back(AppearingEntry);
						}
					}
				}
			}
			else
			{//it isn't a movable entity
				for (auto& pl : Players)
				{//Check if player have this entity
					if (pl.trackedEntities.find(Entity->first) != pl.trackedEntities.end())
					{//Player have this entity. Send regular
						if (IsThereGeneralUpdate)
						{
							m_perPlayerInfo.find(pl.id)->second.UpdateVector.emplace_back(RegEntry);
						}
					}
					else {//Player doesn't have this entity. Send Appearing
						pl.trackedEntities.insert(Entity->first);
						m_perPlayerInfo.find(pl.id)->second.UpdateVector.emplace_back(AppearingEntry);
					}
				}
			}

			Entity = m_gs.m_entities.IterateOverEntities(std::move(iter));
		}
	}

}