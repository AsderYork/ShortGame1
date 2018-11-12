#pragma once
#include "GameSimulation.h"

#include "ServerCommandDispatcher.h"
#include "UpdateSystem_Processor.h"
#include "UpdateSystem_Command.h"
#include "LandscapeSystem_ServerProcessor.h"
#include "ChunkLoadServerDispatcher.h"
#include "LandscapePhysics.h"
#include "CopyableNetworkCommand.h"
#include "GameTimeSystem_ServerProcessor.h"
#include "GameEventsSystem_ServerProcessor.h"


#include <string>
#include <map>
#include <memory>
#include <vector>

#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>

namespace GEM::GameSim
{

	
	class GS_Server
	{

	private:
	
		GameSimulation m_gs;

		bool m_isTimeDirty = true;
		float m_timeSyncFreqInSecs = 10.0f;

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

		void TimeSyncingUpdate();

	protected:
		ServerCommandDispatcher m_commandDispatcher;
		UpdateSystemProcessor m_updateSystemProcessor;
		GameTimeSystem_ServerProcessor m_gameTimeServerProcessor;
		GameEventsSystem_ServerProcessor m_gameEventsServerProcessor;

		ChunkLoadServerDispatcher m_chunkLoadDispatcher;
		LandscapePhysics m_landscapePhysicsController;

		
		/**!
		Sometimes acommand must be sent to every player in the game.
		Such commands are called Global Commands. This vector stores
		GlobalCommands during Simulation tick, and later, such commands are added in
		GatherDataForPlayer for every player.

		*/
		std::vector<std::unique_ptr<CopyableNetworkCommand>> m_globalCommands;

	public:

		GS_Server();

		std::optional<PlayerTicket> NewPlayerRoutine(Player&& player);

		void RemovePlayer(PlayerTicket&& player);

		/**!
		Performs one tick of a simulation
		Fills UpdateList.
		*/
		void Tick(float Delta);
		

		void ReciveDataFromClient(PLAYER_ID_TYPE id, cereal::BinaryInputArchive ar);


		/**!
		Returnes data for a given player.
		This method must be called once and only once for every player right after the tick.
		*/
		std::stringstream GatherDataForPlayer(PLAYER_ID_TYPE id);
		
	};
}

