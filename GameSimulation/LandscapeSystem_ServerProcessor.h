#pragma once
#include "NetworkCommandsProcessor.h"
#include "GameSimulation.h"
#include "LandscapeChunk.h"
#include "LandscapeSystem_Command.h"

#include <functional>
#include <vector>
#include <memory>

namespace GEM::GameSim
{
	class ChunkLoadServerDispatcher;

	/**!
	Performs server-side part of cross-network interaction of LandscapeSystem
	*/
	class LandscapeSystemServerProcessor : public NetworkExchangeProcessor
	{
	private:

		PlayerController& m_playerController;
		ChunkLoadServerDispatcher* m_chunkDispatcher;

	public:

		LandscapeSystemServerProcessor(PlayerController& PlayerController,
			ChunkLoadServerDispatcher* ChunkDispatcher) : m_playerController(PlayerController), m_chunkDispatcher(ChunkDispatcher) {}
		
		virtual bool ApplyCommand(const NetworkCommand * Command, GameTime PacketTime) override;

		virtual void SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command) override;

		virtual std::unique_ptr<NetworkCommand> deserializeCommand(cereal::BinaryInputArchive & ar) override;

		virtual uint8_t getIdOfProcessor() const override;

	};
}