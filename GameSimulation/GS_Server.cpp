#include "GS_Server.h"
#include <sstream>
#include <limits>

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>

namespace GEM::GameSim
{


	std::stringstream GS_Server::GatherDataForPlayer(PLAYER_ID_TYPE id)
	{
		std::stringstream SendStream;
		{
			cereal::BinaryOutputArchive ar(SendStream);

			m_commandDispatcher.GatherResults(
				&(m_perPlayerInfo.find(id)->second.ExchangeHistory), m_gs.getGameTime()).
				SerealizeIn(ar, m_commandDispatcher.getProcessorsTable());
		}

		return SendStream;
	}


	void GS_Server::ReciveDataFromClient(PLAYER_ID_TYPE id, cereal::BinaryInputArchive ar)
	{

		std::vector<ClientCommandPack_Server> ClientCommandPacks;
		try
		{
			while (true)
			{
				ClientCommandPack_Server NewClientCommandPack;

				NewClientCommandPack.SerializeOut(ar, m_commandDispatcher.getProcessorsTable());

				ClientCommandPacks.emplace_back(std::move(NewClientCommandPack));
			}


		}
		catch (cereal::Exception&) {}

		auto& TmpPlayerInfo = m_perPlayerInfo.find(id)->second;
		m_commandDispatcher.ProcessCommands(std::move(ClientCommandPacks), &TmpPlayerInfo.ExchangeHistory);
	}
	

	
	std::pair<UpdateSystemCommand, UpdateSystemCommand>  GS_Server::GetEntityUpdate(std::pair<ENTITY_ID_TYPE, EntityBase*>& Entity)
	{
		auto& mixinvec = Entity.second->getAllMixins();

		UpdateSystemCommand NewPartialUpdate;
		UpdateSystemCommand NewFullUpdate;

		NewPartialUpdate.m_entityID = Entity.first;
		NewFullUpdate.m_entityID = Entity.first;

		for (auto& mixin : mixinvec)
		{
			std::stringstream DataStream;
			if (mixin->NeedsUpdate())
			{
				{
					cereal::BinaryOutputArchive ar(DataStream);
					mixin->SendUpdate(ar, Mixin_base::UpdateReason::REGULAR);
				}
				NewPartialUpdate.m_perMixinUpdates.emplace_back(mixin->getMixinID(), DataStream.str());
			}

			DataStream.str(std::string());
			{
				cereal::BinaryOutputArchive ar(DataStream);
				mixin->SendUpdate(ar, Mixin_base::UpdateReason::APPEAR);
			}
			NewFullUpdate.m_mixins.push_back(mixin->getMixinID());
			NewFullUpdate.m_perMixinUpdates.emplace_back(mixin->getMixinID(), DataStream.str());

		}


		return std::make_pair(NewPartialUpdate, NewFullUpdate);
	}

	void GS_Server::ProcessPlayerSyncingUpdates()
	{
		for (auto& player : m_gs.m_players.getPlayersVector())
		{
			auto& playerIt = m_perPlayerInfo.find(player.id);
			playerIt->second.ExchangeHistory.ReconsiderHistory();
		}
		
	}

	std::optional<PlayerTicket> GS_Server::NewPlayerRoutine(Player && player)
	{
		auto newPlayer = m_gs.m_players.addPlayer(player);
		if (!newPlayer) { return std::nullopt; }
		
		auto [CharPtr, CharId] = m_gs.AddEntity({ Mixin_Movable::MixinID, Mixin_Health::MixinID});
		newPlayer->get().characterPtr = CharPtr;
		newPlayer->get().characterID = CharId;

		m_perPlayerInfo.emplace(newPlayer->get().id, PerPlayerInfo(m_commandDispatcher));


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
						
			
			if (RegUpdate.m_perMixinUpdates.size() == 0)
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
						{//It its in radius of update. Send regular
							if (IsThereGeneralUpdate)
							{
								m_perPlayerInfo.find(pl.id)->second.ExchangeHistory.SendAllreadyPerformedCommand(
									std::make_unique<UpdateSystemCommand>(RegUpdate.m_entityID, RegUpdate.m_perMixinUpdates)
								);
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

							m_perPlayerInfo.find(pl.id)->second.ExchangeHistory.SendAllreadyPerformedCommand(
								std::make_unique<UpdateSystemCommand>(AppearUpdate.m_entityID, AppearUpdate.m_mixins, AppearUpdate.m_perMixinUpdates)
							);
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
							m_perPlayerInfo.find(pl.id)->second.ExchangeHistory.SendAllreadyPerformedCommand(
								std::make_unique<UpdateSystemCommand>(RegUpdate.m_entityID, RegUpdate.m_perMixinUpdates)
							);
						}
					}
					else {//Player doesn't have this entity. Send Appearing
						pl.trackedEntities.insert(Entity->first);
						m_perPlayerInfo.find(pl.id)->second.ExchangeHistory.SendAllreadyPerformedCommand(
							std::make_unique<UpdateSystemCommand>(AppearUpdate.m_entityID, AppearUpdate.m_mixins, AppearUpdate.m_perMixinUpdates)
						);

					}
				}
			}

			Entity = m_gs.m_entities.IterateOverEntities(std::move(iter));
		}
	}

}