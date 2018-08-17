#include "GameTimeSystem_ClientProcessor.h"
#include "GameTimeSystem_Command.h"

namespace GEM::GameSim
{

	bool GameTimeSystem_ClientProcessor::ApplyCommand(const NetworkCommand * Command, GameTime PacketTime)
	{
		auto CommandRecast = static_cast<const GameTimeSystemCommand*>(Command);
		m_gameSim->setGameTime(CommandRecast->NewTime);
		m_gameSim->setGameTimeScale(CommandRecast->NewMultiplayer);

		LOGCATEGORY("DEBUG").debug("NewTime!");

		return true;
	}
	
}