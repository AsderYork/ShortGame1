#pragma once
#include "GameSimulation.h"
#include "UpdateStructures.h"
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
	

		std::map<PLAYER_ID_TYPE, std::vector<UpdateData>> m_UpdateMessages;

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

	public:

		std::optional<PlayerTicket> NewPlayerRoutine(Player&& player);

		void RemovePlayer(PlayerTicket&& player);

		/**!
		Performs one tick of a simulation
		Fills UpdateList.
		*/
		void Tick(float Delta);



		std::vector<UpdateData>& GetUpdatesForPlayer(PLAYER_ID_TYPE id);
		
	};
}

