#pragma once
#include "NetworkCommandsProcessor.h"
#include "GameSimulation.h"

namespace GEM::GameSim
{
	class UpdateSystemProcessor : public NetworkExchangeProcessor
	{
	private:
		GameSimulation * m_gameSim;
	public:

		UpdateSystemProcessor(GameSimulation* GameSim) : m_gameSim(GameSim) {};

		virtual void RollbackCommand(const NetworkCommand * Command) override;
		virtual bool ReapplyCommand(const NetworkCommand * Command) override;
		virtual void ConfirmCommand(const NetworkCommand * Command) override;
		virtual void RejectCommand(const NetworkCommand * Command) override;
		virtual void SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command) override;
		virtual std::unique_ptr<NetworkCommand> deserializeCommand(cereal::BinaryInputArchive & ar) override;

		virtual bool ApplyCommand(const NetworkCommand * Command, GameTime PacketTime) override;
		virtual uint8_t getIdOfProcessor() const override;
	};
}