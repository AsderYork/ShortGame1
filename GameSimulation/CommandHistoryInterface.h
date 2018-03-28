#pragma once
#include "NetworkCommandsProcessor.h"
#include "GameTime.h"
#include <memory>

namespace GEM::GameSim
{

	class HistoryInterface
	{
	public:

		/**!
		Inserts new command in history
		*/
		virtual void InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime) = 0;



		virtual void ReconsiderHistory() {}
	};
}