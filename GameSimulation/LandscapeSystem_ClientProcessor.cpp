#include "LandscapeSystem_ClientProcessor.h"
#include "LandscapeSystem_Command.h"
#include "ChunkLoadClientDispatcher.h"

namespace GEM::GameSim
{
	bool LandscapeSystemClientProcessor::ApplyCommand(const NetworkCommand * Command, GameTime PacketTime)
	{
		auto CommandRecast = static_cast<const LandscapeSystemCommand_Responce*>(Command);

		auto UnpackedChunks = CommandRecast->pack.UnpackLandscapeChunks();

		for (auto& chnk : UnpackedChunks)
		{
			auto chnkPos = chnk.getPosition();
			auto NewChunkPlace = m_dispatcher->addNewChunk(chnkPos.first, chnkPos.second);
			NewChunkPlace->chunk = chnk;
			NewChunkPlace->isConfirmed = true;
		}
		return true;
	}

	void LandscapeSystemClientProcessor::RollbackCommand(const NetworkCommand * Command)
	{
	}

	bool LandscapeSystemClientProcessor::ReapplyCommand(const NetworkCommand * Command)
	{
		return false;
	}

	void LandscapeSystemClientProcessor::ConfirmCommand(const NetworkCommand * Command)
	{

	}

	void LandscapeSystemClientProcessor::RejectCommand(const NetworkCommand * Command)
	{
	}

	void LandscapeSystemClientProcessor::SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command)
	{
		auto CommandRecast = static_cast<const LandscapeSystemCommand_RequestChunks*>(Command);
		ar((*CommandRecast));
	}

	std::unique_ptr<NetworkCommand> LandscapeSystemClientProcessor::deserializeCommand(cereal::BinaryInputArchive & ar)
	{
		auto NewCommand = std::make_unique<LandscapeSystemCommand_Responce>();
		ar((*NewCommand));
		return NewCommand;
	}

	uint8_t LandscapeSystemClientProcessor::getIdOfProcessor() const
	{
		return 1;
	}

}