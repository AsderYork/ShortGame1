#pragma once
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "GS_Player.h"



namespace GEM::GameSim
{
	using GS_Enitity_ID_Type = uint32_t;
	/**!
	Some stuff about entities. We really need some default value for them. And it will be zero.
	No entity should use that value. NEVER!
	*/

	class GameSimulation {
	private:
		/**!
		Holds data about connected players and what entity do they hold right now.
		Server should be responsible, to make it so that every hold only entities, that it's allowed to hold
		Transitions about what object exactly is holded right now can be performed only by the server.
		*/
		std::unordered_map<GS_PlayerID_Type, GS_Enitity_ID_Type> m_players;
		const unsigned int m_maxPlayers;

	public:
		
		GameSimulation(unsigned int maxPlayers) : m_maxPlayers(maxPlayers) {
		}
		/**!
		Adds a player to a simulation
		If a player with given ID allready exists, the parameter will stay valid, otherwise it will be moved.
		\returns returns player ID if it's been added, and nothing otherwise;
		*/
		bool AddPlayer(PlayerData&& Player);

		/**!
		Returns a player name by it's ID. Returns empty string if and only if there is no such player.
		For every existing player their non-zero-sized name will be returned
		*/
		std::string getPlayerName(const GS_PlayerID_Type ID);

	};
}