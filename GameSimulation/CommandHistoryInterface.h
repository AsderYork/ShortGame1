#pragma once
#include "NetworkCommandsProcessor.h"
#include "GameTime.h"
#include <array>
#include <memory>

namespace GEM::GameSim
{

	class HistoryInterface
	{
	public:
		/**!
		Every processor have unique ID and we have less then 256 processors.
		So we just use this array as table for quick search of processors
		*/
		std::array<NetworkExchangeProcessor*, 256> m_processors;

		/**!
		Inserts new command in history
		*/
		virtual void InjectCommand(std::unique_ptr<NetworkCommand>&& command, GameTime PacketTime) = 0;



		virtual void ReconsiderHistory() {}
	};
}