#include "GameEventsSystem_ServerProcessor.h"

namespace GEM::GameSim
{

	bool GameEventsSystem_ServerProcessor::ApplyCommand(const NetworkCommand* Command, GameTime PacketTime) {
		auto CastedCommand = static_cast<const GameEventsSystem_Command*>(Command);

		if (std::holds_alternative<GameEventsSystem_Command::InitalPack>(CastedCommand->state))
		{
			auto InitialPack = std::get<GameEventsSystem_Command::InitalPack>(CastedCommand->state);
			InitialPack.m_eventId;
		}
		return false;
	}


}

