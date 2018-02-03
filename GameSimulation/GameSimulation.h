#pragma once
#include <unordered_map>
#include <optional>

#include "EntitiesBase.h"

#include <stack>



namespace GEM::GameSim
{	
	class GameSimulation {
	private:
		/**!
		Holds data about connected players and what entities do they hold right now.
		Server is responsible for assigning entities to a player.
		Player can send commands only for entities, they own.
		*/
	//	std::unordered_map<PlayerIdentityData, std::list<GS_Enitity_ID_Type>> m_players;

		/**!
		Game simulations can limit the number of players
		*/
		const unsigned int m_maxPlayers;
	public:
		
		GameSimulation(unsigned int maxPlayers) : m_maxPlayers(maxPlayers) {
		}
		/**!
		Adds a player to a simulation
		\param[in] ID ID of a player, that should be added. Insert will not occur, if ID is not unique
		\param[in] name UserName. Should be non-empty. Users must guarantee, that Names do correspond to ID's and that they also unique
		\returns returns player ID if it's been added
		*/
		//bool AddPlayer(GS_PlayerID_Type ID, const std::string name);

		/**!
		Returns a player name by it's ID. Returns empty string if and only if there is no such player.
		For every existing player their non-zero-sized name will be returned
		*/
	//	std::string getPlayerName(const GS_PlayerID_Type ID);

	};
}