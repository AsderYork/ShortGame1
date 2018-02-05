#include "GS_PlayerController.h"

namespace GEM::GameSim
{

	bool GameSim_PlayerController::AddPlayer(PLAYER_ID_TYPE id, std::string Username)
	{
		auto UsernameMapResult = m_idUsernameMap.insert({ Username, id });
		if (UsernameMapResult.second)
		{
			if (m_playerMap.insert({ id,
				PlayerData(std::set<ENTITY_ID_TYPE>(), UsernameMapResult.first) }).second)
			{
				return true;
			}
			//If we wern't able to insert in player map, that means the ID
			//is not unique. So rollback it all
			m_idUsernameMap.erase(UsernameMapResult.first);
		}

		return false;
	}

	bool GameSim_PlayerController::RemovePlayer(PLAYER_ID_TYPE id)
	{
		auto FindResult = m_playerMap.find(id);
		if (FindResult == m_playerMap.end()) { return false; }
		m_idUsernameMap.erase(FindResult->second.UsernameIDMapIter);
		m_playerMap.erase(FindResult);
		return true;
	}

	int GameSim_PlayerController::GetPlayerCount() const
	{
		return static_cast<int>(m_playerMap.size());
	}

	std::optional<PLAYER_ID_TYPE> GameSim_PlayerController::GetIDbyName(const std::string & name) const
	{
		auto& retfind = m_idUsernameMap.find(name);
		if (retfind == m_idUsernameMap.end()) { return std::nullopt; }
		return std::optional<PLAYER_ID_TYPE>(retfind->second);
	}

	std::optional<std::string> GameSim_PlayerController::GetNameByID(PLAYER_ID_TYPE id) const
	{
		auto& retfind = m_playerMap.find(id);
		if (retfind == m_playerMap.end()) { return std::nullopt; }
		return std::optional<std::string>(retfind->second.UsernameIDMapIter->first);
	}

	std::set<ENTITY_ID_TYPE>* GameSim_PlayerController::GetEntityListByID(PLAYER_ID_TYPE id)
	{
		auto& retfind = m_playerMap.find(id);
		if (retfind != m_playerMap.end())
		{
			return &(retfind->second.EntityList);
		}
		return nullptr;
	}

}