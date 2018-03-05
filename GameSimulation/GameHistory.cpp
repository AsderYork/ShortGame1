#include "GameHistory.h"

namespace GEM::GameSim
{
	SyncingUpdate_Packet GameHistoryController::AddNewUpdate(const StateChange Update)
	{
		if (m_lastID == std::numeric_limits<SYNCING_EVENT_ID>::max())
		{
			throw std::exception("Event ID counter reached maximum possible number and overflow is not supported! How long have you've been playing?");
		}
		AddNewUpdateWithID(std::make_unique<StateChange>(Update), m_lastID);

		return SyncingUpdate_Packet(Update, m_lastID++);
	}
}
