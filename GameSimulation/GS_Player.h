#pragma once
#include <cstdint>
#include <string>
#include "EntitiesBase.h"

namespace GEM::GameSim
{
	/**!
	Players have ID. The ID is unique for EVERY player connected or not.
	*/
	using GS_PlayerID_Type = uint32_t;

	struct PlayerData
	{
		const std::string m_userName;
		GS_Enitity_ID_Type m_ent;

		PlayerData(const std::string userName, const GS_Enitity_ID_Type OwnedID = ENTITY_ID_NONE) : m_userName(userName), m_ent(OwnedID) {}
	};

}