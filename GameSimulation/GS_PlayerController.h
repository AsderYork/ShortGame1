#pragma once
#include <unordered_map>
#include <set>
#include <map>
#include <string>
#include <optional>

namespace GEM::GameSim
{
	using PLAYER_ID_TYPE = uint32_t;

	//Temporary
	using ENTITY_ID_TYPE = uint32_t;

	/**!
	Controlls players.
	Various info about them, like their username, and entities they are allowed to conmmand
	*/
	class GameSim_PlayerController
	{
	private:
		using UsernameMapIter = std::map<std::string, PLAYER_ID_TYPE>::iterator;
		struct PlayerData {
			std::set<ENTITY_ID_TYPE> EntityList;
			UsernameMapIter UsernameIDMapIter;
			PlayerData(std::set<ENTITY_ID_TYPE> list, UsernameMapIter iter) :
				EntityList(list), UsernameIDMapIter(iter) {}
		};

		std::unordered_map<PLAYER_ID_TYPE, PlayerData> m_playerMap;
		std::map<std::string, PLAYER_ID_TYPE> m_idUsernameMap;

	public:

		/**!
		Adds a player.
		\param[in] id id of a player, to be added. Must be unique for every player
		\param[in] Username Username of the player. Must be unique for every player
		\returns Returns true if player is added successfully, false otherwise
		*/
		bool AddPlayer(PLAYER_ID_TYPE id, std::string Username);
		
		/**!
		Removes a Player
		\param[in] id ID of the player, that should be removed
		*/
		bool RemovePlayer(PLAYER_ID_TYPE id);
		
		/**!
		Get the number of players currently in
		*/
		int GetPlayerCount() const;

		/**!
		Returns playerID for a given name, if there is one
		*/
		std::optional<PLAYER_ID_TYPE> GetIDbyName(const std::string& name) const;

		/**!
		Returns player name by it's ID, if there is one
		*/
		std::optional<std::string> GetNameByID(PLAYER_ID_TYPE id) const;

		/**!
		Returns a pointer to a list of entities, owned by this player
		if this player exists. Otherwise null pointer will be returned.
		*/
		std::set<ENTITY_ID_TYPE>* GetEntityListByID(PLAYER_ID_TYPE id);

	};
}