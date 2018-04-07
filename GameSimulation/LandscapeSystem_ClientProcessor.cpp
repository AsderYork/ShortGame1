#include "LandscapeSystem_ClientProcessor.h"
#include "LandscapeSystem_Command.h"

namespace GEM::GameSim
{
	bool LandscapeSystemClientProcessor::ApplyCommand(const NetworkCommand * Command, GameTime PacketTime)
	{
		return false;
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
		auto NewCommand = std::make_unique<LandscapeSystemCommand_RequestChunks>();
		ar((*NewCommand));
		return NewCommand;
	}

	uint8_t LandscapeSystemClientProcessor::getIdOfProcessor() const
	{
		return 1;
	}

}