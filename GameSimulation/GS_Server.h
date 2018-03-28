#pragma once
#include "GameSimulation.h"
#include "UpdateStructures.h"
#include "GameHistory.h"

#include "ServerCommandDispatcher.h"
#include "UpdateSystem_Processor.h"

#include <string>
#include <map>

#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>

namespace GEM::GameSim
{

	
	class GS_Server
	{
	public:

		/**!
		Holds everything, that should be sended to a player
		*/
		struct PlayerUpdatePack
		{
			GameTime UpdateTime;
			std::vector<UpdateData> updates;
			OutOfSync_Packet OOS;
			InSync_Packet InSync;

			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(UpdateTime, OOS, InSync, updates);
			}
		};
	private:
	

		struct PerPlayerInfo
		{
			std::vector<UpdateData> UpdateVector;
			std::vector<std::pair<SyncingUpdate_Packet, GameTime>> SynchroUpdates;
			InSync_Packet currentInSync;
			OutOfSync_Packet currentOOS;
			ServerHistory ExchangeHistory;

			PerPlayerInfo(ServerCommandDispatcher& Dispatcher) : ExchangeHistory(Dispatcher) {}
		};
		std::map<PLAYER_ID_TYPE, PerPlayerInfo> m_perPlayerInfo;

		GameSimulation m_gs;

		/**!
		Entities, that have Mixin_Movable in them, will be updated for a given player,
		only if they are within this radius from a given player.
		Entities that don't have movable will allways be updated.
		*/
		float m_playerUpldateRadius = 1000.0f;

		/**!
		Gathers entity updates;		
		*/
		std::pair<EntityRegularUpdate, EntityAppearingUpdate>  GetEntityUpdate(std::pair<ENTITY_ID_TYPE, EntityBase*>& Entity);

		void ProcessPlayerSyncingUpdates();

	protected:
		ServerCommandDispatcher m_commandDispatcher;
		UpdateSystemProcessor m_updateSystemProcessor;

	public:

		GS_Server() : m_updateSystemProcessor(&m_gs)
		{
			m_commandDispatcher.AddProcessor(&m_updateSystemProcessor);
		}

		std::optional<PlayerTicket> NewPlayerRoutine(Player&& player);

		void RemovePlayer(PlayerTicket&& player);

		/**!
		Performs one tick of a simulation
		Fills UpdateList.
		*/
		void Tick(float Delta);

		void ReciveSynchroUpdatesFromClient(PLAYER_ID_TYPE id, cereal::BinaryInputArchive ar);

		/**!
		Returns a block of data, that should be sended to a given player.
		This method should be called only once for every player per tick.
		*/
		PlayerUpdatePack GatherDataForPlayer(PLAYER_ID_TYPE id);

		std::stringstream GatherOtherDataForPlayer(PLAYER_ID_TYPE id);
		
	};
}

