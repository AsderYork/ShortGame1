#include "GS_Server.h"
#include "LogHelper.h"

#include <sstream>
#include <limits>

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>

namespace GEM::GameSim
{

	GS_Server::GS_Server() :
		m_updateSystemProcessor(&m_gs),
		m_chunkLoadDispatcher(m_gs.m_players),
		m_landscapePhysicsController((&m_gs.m_physics))
	{
		m_commandDispatcher.AddProcessor(&m_updateSystemProcessor);
		m_commandDispatcher.AddProcessor(&(m_chunkLoadDispatcher.getProcessor()));
		m_chunkLoadDispatcher.Start();

		m_chunkLoadDispatcher.getStorage().RegisterListener(&m_landscapePhysicsController);
	}


	std::stringstream GS_Server::GatherDataForPlayer(PLAYER_ID_TYPE id)
	{
		std::stringstream SendStream;
		{
			cereal::BinaryOutputArchive ar(SendStream);

			m_commandDispatcher.GatherResults(
				&(m_gs.m_players.getPlayer(id)->additional_data->ExchangeHistory), m_gs.getGameTime()).
				SerealizeIn(ar, m_commandDispatcher.getProcessorsTable());
		}

		std::stringstream SizeStream;
		{
			cereal::BinaryOutputArchive ar(SizeStream);
			ar(static_cast<std::size_t>(SendStream.gcount()));
		}

		SendStream.str(SizeStream.str() + SendStream.str());
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

		auto& TmpPlayerInfo = m_gs.m_players.getPlayer(id)->additional_data;
		m_commandDispatcher.ProcessCommands(std::move(ClientCommandPacks), &TmpPlayerInfo->ExchangeHistory, m_gs.getGameTime());
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
			
	}

	std::optional<PlayerTicket> GS_Server::NewPlayerRoutine(Player && player)
	{
		auto newPlayer = m_gs.m_players.addPlayer(std::move(player));
		if (!newPlayer) { return std::nullopt; }
		
		auto [CharPtr, CharId] = m_gs.AddEntity({ Mixin_Movable::MixinID, Mixin_Health::MixinID});
		newPlayer->get().characterPtr = CharPtr;
		newPlayer->get().characterID = CharId;

		//Set player's starting pos
		m_chunkLoadDispatcher.ForceChunkCreation(0, 0);

		btVector3 Start(0, 512, 0), End(0, 0, 0);
		btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);

		float Height = 20.0f;
		m_gs.m_physics.getWorld()->rayTest(Start, End, RayCallback);
		if (RayCallback.hasHit())
		{
			Height = static_cast<float>(RayCallback.m_hitPointWorld.y());
		}

		dynamic_cast<Mixin_Movable*>(CharPtr->GetMixinByID(Mixin_Movable::MixinID))->Shift(0.0f, Height, 0.0f);

		auto LoaderID = m_chunkLoadDispatcher.getChunkController().createNewLoader(
			[CharPtr]() {return dynamic_cast<Mixin_Movable*>(CharPtr->GetMixinByID(Mixin_Movable::MixinID))->getPos(); }
		);

		newPlayer->get().additional_data = std::make_unique<Player::ServerRelatedPart>(m_commandDispatcher, LoaderID, newPlayer->get().id);

		
		return newPlayer;
	}

	void GS_Server::RemovePlayer(PlayerTicket && player)
	{
		m_chunkLoadDispatcher.getChunkController().RemoveLoader(player.get().additional_data->MapLoaderId);

		m_gs.m_entities.RemoveEntity(player.get().characterID);
		m_gs.m_players.RemovePlayer(std::move(player));
	}

	void  GS_Server::Tick(float Delta)
	{
		m_chunkLoadDispatcher.ProcessChunks();
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
																
								pl.additional_data->ExchangeHistory.SendAllreadyPerformedCommand(
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

							pl.additional_data->ExchangeHistory.SendAllreadyPerformedCommand(
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
							pl.additional_data->ExchangeHistory.SendAllreadyPerformedCommand(
								std::make_unique<UpdateSystemCommand>(RegUpdate.m_entityID, RegUpdate.m_perMixinUpdates)
							);
						}
					}
					else {//Player doesn't have this entity. Send Appearing
						pl.trackedEntities.insert(Entity->first);
						pl.additional_data->ExchangeHistory.SendAllreadyPerformedCommand(
							std::make_unique<UpdateSystemCommand>(AppearUpdate.m_entityID, AppearUpdate.m_mixins, AppearUpdate.m_perMixinUpdates)
						);

					}
				}
			}

			Entity = m_gs.m_entities.IterateOverEntities(std::move(iter));
		}

	}

}