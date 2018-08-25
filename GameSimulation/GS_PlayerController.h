#pragma once
#include "Player_id_type.h"
#include "ServerCommandDispatcher.h"
#include "GS_EntityController.h"

#include <string>
#include <optional>
#include <set>
#include <memory>

namespace GEM::GameSim
{
	class PlayerController;

	

	struct Player
	{
		std::string playerName;

		//!ID of a player character entity
		std::weak_ptr<EntityBase> characterPtr;
		ENTITY_ID_TYPE characterID;

		struct ServerRelatedPart
		{
			ServerHistory ExchangeHistory;
			uint64_t MapLoaderId;

			ServerRelatedPart(ServerCommandDispatcher& Dispatcher, uint64_t LoaderID, PLAYER_ID_TYPE playerid) :
				ExchangeHistory(Dispatcher, playerid), MapLoaderId(LoaderID) {}
		};

		std::unique_ptr<ServerRelatedPart> additional_data;

		//!Set of all entities, that should be updated for this client.
		mutable std::set<ENTITY_ID_TYPE> trackedEntities;

		
		PLAYER_ID_TYPE id;	

		Player(std::string name) : playerName(name) {};
		Player() {}

	};

	/**!
	Outer interface for players
	Used to hide the optional part of a player, to ease player removal but
	not expose it to a user

	Player tickets livetime MUST NOT exceed a lifetime of PlayerController!
	*/
	class PlayerTicket
	{
	private:

		struct StaticMemoryPack
		{
			std::size_t m_playerIndex;
			PlayerController* m_contr = nullptr;
			StaticMemoryPack* m_next = nullptr;//Tickets secretly forms a linked list inside a vector!
			StaticMemoryPack* m_prev = nullptr;

			StaticMemoryPack(std::size_t p, PlayerController* controller, StaticMemoryPack* prev);
			~StaticMemoryPack();

		};

		std::unique_ptr<StaticMemoryPack> m_internal;
		

		friend class PlayerController;
		PlayerTicket(std::size_t p, PlayerController* controller, StaticMemoryPack* prev);
	public:
		/**!
		Returns a reference to a player.
		This values should NOT be stored becouse it might become invalidated after every player removal
		*/
		Player & get();

		PlayerTicket(const PlayerTicket&) = delete;
		PlayerTicket(PlayerTicket&&) = default;
		
	};

	/**!
	Controlls players.
	Various info about them, like their username
	*/
	class PlayerController
	{
	private:
		friend class PlayerTicket;
		std::vector<Player> m_players;
		std::size_t m_maxPlayers;

		PLAYER_ID_TYPE m_lastGivenID = 0;

		PlayerTicket::StaticMemoryPack* m_listHead = nullptr;//Tickets forms a linked list, which allow them to perform shifts, when one of them gets removed
		PlayerTicket::StaticMemoryPack* m_listEnd = nullptr;
	public:
		PlayerController(std::size_t MaxPlayers = 100) : m_maxPlayers(MaxPlayers)
		{
			m_players.reserve(MaxPlayers);
		}

		/**!
		Adds new player.
		\returns returns a ticket to a newly created player. If there is no place for a new player, nullopt is returned
		*/
		std::optional<PlayerTicket> addPlayer(Player Player);

		void RemovePlayer(PlayerTicket&& ticket);

		inline const decltype(m_players)& getPlayersVector() { return m_players; }

		/**!
		Returns player by it's ID. If no player is found with given id, nullptr is returned
		*/
		Player* getPlayer(PLAYER_ID_TYPE id);
	};
}