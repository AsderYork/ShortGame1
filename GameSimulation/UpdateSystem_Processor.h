#pragma once
#include "NetworkCommandsProcessor.h"
#include "GameSimulation.h"
#include "UpdateSystem_Command.h"

namespace GEM::GameSim
{
	class UpdateSystemProcessor : public NetworkExchangeProcessorSingleCommandSerialization<UpdateSystemCommand>
	{
	private:
		GameSimulation * m_gameSim;
	public:

		UpdateSystemProcessor(GameSimulation* GameSim) : m_gameSim(GameSim) {};

		virtual bool ApplyCommand(const NetworkCommand * Command, GameTime PacketTime) override;
		virtual uint8_t getIdOfProcessor() const override;
	};
}