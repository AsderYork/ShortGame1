#pragma once
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "GS_Player.h"



namespace GEM::GameSim
{	
	class GameSimulation {
	private:
		/**!
		Holds data about connected players and what entity do they hold right now.
		Server should be responsible, to make it so that every hold only entities, that it's allowed to hold
		Transitions about what object exactly is holded right now can be performed only by the server.
		*/
		std::unordered_map<GS_PlayerID_Type, PlayerData> m_players;
		const unsigned int m_maxPlayers;

	public:
		
		GameSimulation(unsigned int maxPlayers) : m_maxPlayers(maxPlayers) {
		}
		/**!
		Adds a player to a simulation
		\param[in] ID ID of a player, that should be added. Insert will not occur, if ID is not unique
		\param[in] name UserName. Should be non-empty. Users must guarantee, that Names do correspond to ID's and that they also unique
		\param[in] entID ID of an entity, that is directly controlled by this player. Don't pass this paramter, if
		\returns returns player ID if it's been added
		*/
		bool AddPlayer(GS_PlayerID_Type ID, const std::string name, GS_Enitity_ID_Type entID=0);

		/**!
		Returns a player name by it's ID. Returns empty string if and only if there is no such player.
		For every existing player their non-zero-sized name will be returned
		*/
		std::string getPlayerName(const GS_PlayerID_Type ID);

	};
}