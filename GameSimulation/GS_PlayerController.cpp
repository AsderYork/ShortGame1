#include "GS_PlayerController.h"
#include <limits>

namespace GEM::GameSim
{

	PlayerTicket::StaticMemoryPack::StaticMemoryPack(std::size_t p, PlayerController * controller, StaticMemoryPack * prev)
		: m_playerIndex(p), m_contr(controller), m_prev(prev)
	{
		if (m_contr->m_listHead == nullptr) { m_contr->m_listHead = this; }
		if (m_contr->m_listEnd != nullptr) { m_contr->m_listEnd->m_next = this; }
		m_contr->m_listEnd = this;
	}

	PlayerTicket::StaticMemoryPack::~StaticMemoryPack()
	{
		if (m_contr->m_listHead == this) { m_contr->m_listHead = m_next; }
		if (m_contr->m_listEnd == this) { m_contr->m_listEnd = m_prev; }

		if (m_prev != nullptr) { m_prev->m_next = m_next; }
		
	}



	PlayerTicket::PlayerTicket(std::size_t p, PlayerController * controller, StaticMemoryPack * prev) :
		m_internal(std::make_unique<StaticMemoryPack>(p, controller, prev))
	{
		
	}
	Player & PlayerTicket::get()
	{
		return m_internal->m_contr->m_players[m_internal->m_playerIndex];
	}


	std::optional<PlayerTicket> PlayerController::addPlayer(Player Player)
	{
		if(m_players.size() == m_maxPlayers){return std::nullopt;}
		Player.id = m_lastGivenID;


		//THIS METHOD OF GIVING IDS IS TEMPORRARY! IT IS BROKEN!
		if (m_lastGivenID == std::numeric_limits<PLAYER_ID_TYPE>::max())
		{
			m_lastGivenID = 0;
		}
		else
		{
			m_lastGivenID++;
		}


		m_players.emplace_back(Player);
		return PlayerTicket(m_players.size()-1, this, m_listEnd);

		
	}

	void PlayerController::RemovePlayer(PlayerTicket && ticket)
	{
		m_players.erase(m_players.begin() + ticket.m_internal->m_playerIndex);
		auto NextTcket = ticket.m_internal->m_next;

		while (NextTcket != nullptr)
		{
			NextTcket->m_playerIndex--;
			NextTcket = NextTcket->m_next;
		}
	}



	

}