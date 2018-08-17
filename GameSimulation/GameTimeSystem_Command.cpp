#include "GameTimeSystem_Command.h"

namespace GEM::GameSim
{
	std::unique_ptr<NetworkCommand> GameTimeSystemCommand::copy()
	{
		return std::make_unique<GameTimeSystemCommand>(*this);
	}
}