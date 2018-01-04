#include "GameSimulation.h"

namespace GEM::GameSim
{
	bool GameSimulation::AddPlayer(PlayerData&& Player)
	{//Assumes single-thread

		if (m_players.size() > m_maxPlayers)
		{
			return false;//Container is full!
		}

		auto retval = m_players.emplace(std::move(Player));
		
		return retval.second;
	}
	std::string GameSimulation::getPlayerName(const GS_PlayerID_Type ID)
	{

		auto Retval = m_players.find(ID);
	}
}
