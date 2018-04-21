#include "stdafx.h"
#include "LandscapeSystem_ClientProcessor.h"
#include "LandscapeSystem_Command.h"
#include "ChunkLoadClientDispatcher.h"

namespace GEM
{
	bool LandscapeSystemClientProcessor::ApplyCommand(const GameSim::NetworkCommand * Command, GameSim::GameTime PacketTime)
	{
		auto CommandRecast = static_cast<const GameSim::LandscapeSystemCommand_Responce*>(Command);

		auto UnpackedChunks = CommandRecast->pack.UnpackLandscapeChunks();

		for (auto& chnk : UnpackedChunks)
		{
			auto chnkPos = chnk.getPosition();
			auto NewChunkPlace = m_dispatcher->m_chunks.addChunk(chnkPos.first, chnkPos.second, [&](GameSim::LandscapeChunk* ch) {*ch = chnk; });
		}
		return true;
	}

	void LandscapeSystemClientProcessor::RollbackCommand(const GameSim::NetworkCommand * Command)
	{
	}

	bool LandscapeSystemClientProcessor::ReapplyCommand(const GameSim::NetworkCommand * Command)
	{
		return false;
	}

	void LandscapeSystemClientProcessor::ConfirmCommand(const GameSim::NetworkCommand * Command)
	{

	}

	void LandscapeSystemClientProcessor::RejectCommand(const GameSim::NetworkCommand * Command)
	{
	}

	void LandscapeSystemClientProcessor::SerializeCommand(cereal::BinaryOutputArchive & ar, const GameSim::NetworkCommand * Command)
	{
		auto CommandRecast = static_cast<const GameSim::LandscapeSystemCommand_RequestChunks*>(Command);
		ar((*CommandRecast));
	}

	std::unique_ptr<GameSim::NetworkCommand> LandscapeSystemClientProcessor::deserializeCommand(cereal::BinaryInputArchive & ar)
	{
		auto NewCommand = std::make_unique<GameSim::LandscapeSystemCommand_Responce>();
		ar((*NewCommand));
		return NewCommand;
	}

	uint8_t LandscapeSystemClientProcessor::getIdOfProcessor() const
	{
		return 1;
	}

	void LandscapeSystemClientProcessor::EndNetworkProcessing()
	{
		m_dispatcher->m_chunks.generateMeshesForUnpreparedChunks();
	}

}