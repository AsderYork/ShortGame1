#include "LandscapeSystem_ServerProcessor.h"
#include "LandscapeSystem_Command.h"

namespace GEM::GameSim
{

	bool LandscapeSystemServerProcessor::ApplyCommand(const NetworkCommand * Command, GameTime PacketTime)
	{
		return false;
	}

	void LandscapeSystemServerProcessor::RollbackCommand(const NetworkCommand * Command)
	{
	}

	bool LandscapeSystemServerProcessor::ReapplyCommand(const NetworkCommand * Command)
	{
		return false;
	}

	void LandscapeSystemServerProcessor::ConfirmCommand(const NetworkCommand * Command)
	{
	}

	void LandscapeSystemServerProcessor::RejectCommand(const NetworkCommand * Command)
	{
	}

	void LandscapeSystemServerProcessor::SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command)
	{
		auto CommandRecast = static_cast<const LandscapeSystemCommand_RequestChunks*>(Command);
		ar((*CommandRecast));
	}

	std::unique_ptr<NetworkCommand> GEM::GameSim::LandscapeSystemServerProcessor::deserializeCommand(cereal::BinaryInputArchive & ar)
	{
		auto NewCommand = std::make_unique<LandscapeSystemCommand_RequestChunks>();
		ar((*NewCommand));
		return NewCommand;
	}

	uint8_t GEM::GameSim::LandscapeSystemServerProcessor::getIdOfProcessor() const
	{
		return 1;
	}

}