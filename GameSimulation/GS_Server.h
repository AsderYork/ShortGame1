#pragma once
#include "GameSimulation.h"

#include "ServerCommandDispatcher.h"
#include "UpdateSystem_Processor.h"
#include "UpdateSystem_Command.h"
#include "LandscapeSystem_ServerProcessor.h"


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

	private:
	

		struct PerPlayerInfo
		{
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
		std::pair<UpdateSystemCommand, UpdateSystemCommand>  GetEntityUpdate(std::pair<ENTITY_ID_TYPE, EntityBase*>& Entity);

		void ProcessPlayerSyncingUpdates();

	protected:
		ServerCommandDispatcher m_commandDispatcher;
		UpdateSystemProcessor m_updateSystemProcessor;
		LandscapeSystemServerProcessor m_ladnscapeProcessor;

	public:

		GS_Server() : m_updateSystemProcessor(&m_gs)
		{
			m_commandDispatcher.AddProcessor(&m_updateSystemProcessor);
			m_commandDispatcher.AddProcessor(&m_ladnscapeProcessor);
		}

		std::optional<PlayerTicket> NewPlayerRoutine(Player&& player);

		void RemovePlayer(PlayerTicket&& player);

		/**!
		Performs one tick of a simulation
		Fills UpdateList.
		*/
		void Tick(float Delta);
		

		void ReciveDataFromClient(PLAYER_ID_TYPE id, cereal::BinaryInputArchive ar);


		std::stringstream GatherDataForPlayer(PLAYER_ID_TYPE id);
		
	};
}

