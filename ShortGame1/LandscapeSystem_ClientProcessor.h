#pragma once
#include <NetworkCommandsProcessor.h>
#include <GameSimulation.h>

namespace GEM
{

	class ChunkLoadClientDispatcher;

	class LandscapeSystemClientProcessor : public GameSim::NetworkExchangeProcessor
	{
	private:
		ChunkLoadClientDispatcher* m_dispatcher;
	public:
		LandscapeSystemClientProcessor(ChunkLoadClientDispatcher* dispatcher) : m_dispatcher(dispatcher) {}

		virtual bool ApplyCommand(const GameSim::NetworkCommand * Command, GameSim::GameTime PacketTime) override;
		virtual void RollbackCommand(const GameSim::NetworkCommand * Command) override;
		virtual bool ReapplyCommand(const GameSim::NetworkCommand * Command) override;
		virtual void ConfirmCommand(const GameSim::NetworkCommand * Command) override;
		virtual void RejectCommand(const GameSim::NetworkCommand * Command) override;
		virtual void SerializeCommand(cereal::BinaryOutputArchive & ar, const GameSim::NetworkCommand * Command) override;
		virtual std::unique_ptr<GameSim::NetworkCommand> deserializeCommand(cereal::BinaryInputArchive & ar) override;
		virtual uint8_t getIdOfProcessor() const override;
		virtual void EndNetworkProcessing() override;
	};
}