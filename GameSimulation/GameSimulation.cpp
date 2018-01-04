#include "GameSimulation.h"
#include <cassert>

namespace GEM::GameSim
{
	bool GameSimulation::AddPlayer(GS_PlayerID_Type ID, const std::string name, GS_Enitity_ID_Type entID)
	{//Assumes single-thread
		assert(name.size() != 0);//Name should not be empty!

		if (m_players.size() + 1 > m_maxPlayers)
		{
			return false;//Container is full!
		}

		auto retval = m_players.emplace(ID, PlayerData(name, entID));
		
		return retval.second;
	}
	std::string GameSimulation::getPlayerName(const GS_PlayerID_Type ID)
	{

		//auto Retval = m_players.find(ID);

		//if (Retval == m_players.end())
		{ return std::string(); }

		//return Retval->second.m_userName;
	}
}
