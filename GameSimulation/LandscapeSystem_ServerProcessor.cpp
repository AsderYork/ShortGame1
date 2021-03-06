#include "LandscapeSystem_ServerProcessor.h"
#include "ChunkLoadServerDispatcher.h"

namespace GEM::GameSim
{

	bool LandscapeSystemServerProcessor::ApplyCommand(const NetworkCommand * Command, GameTime PacketTime)
	{
		auto RecastedCommand = static_cast<const LandscapeSystemCommand_RequestChunks*>(Command);

		if (RecastedCommand->requests.size() == 0) { return true; }

		auto Player = m_playerController.getPlayer(getCurrentPlayer());

		auto respond = std::make_unique<LandscapeSystemCommand_Responce>();
		std::vector<const LandscapeChunk*> ChunkPtrs;

		auto VisChunks = m_chunkDispatcher->getChunkController().getVisibleChunksOfLoader(Player->additional_data->MapLoaderId);

		int PorcessedReqChunks = 0;
		//We assume, that chunks in request are sorted exactly as in VisChunks;
		for (auto& reqchunk : VisChunks)
		{
			if (PorcessedReqChunks >= RecastedCommand->requests.size()) { break; }

			auto& TmpChunkRequest = RecastedCommand->requests[PorcessedReqChunks];
			if ((TmpChunkRequest.x != reqchunk.first) || (TmpChunkRequest.z != reqchunk.second))
			{
				continue;//Search untill visible chunk is also a requested one
			}

			//If requested chunk, that is also visible is found
			PorcessedReqChunks++;

			auto RequestedChunk = m_chunkDispatcher->getStorage().getChunk(TmpChunkRequest.x, TmpChunkRequest.z);
			if (RequestedChunk == nullptr)
			{
				LOGCATEGORY("LandscapeSystem_ServerProcessor").warn("Client requested chunk (%i:%i) that is no avaliable!", TmpChunkRequest.x, TmpChunkRequest.z);
				respond->requests.push_back(GameSim::ChunkRequest(TmpChunkRequest.x, TmpChunkRequest.z, 0));
			}
			else
			{
				respond->requests.push_back(GameSim::ChunkRequest(TmpChunkRequest.x, TmpChunkRequest.z, RequestedChunk->chunk.getVersion()));
				ChunkPtrs.push_back(&(RequestedChunk->chunk));
			}
		}

		respond->pack = LandscapeChunkPack(ChunkPtrs);
		respond->m_header = 1;
		Player->additional_data->ExchangeHistory.SendAllreadyPerformedCommand(std::move(respond));

		return true;
	}

	
	void LandscapeSystemServerProcessor::SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command)
	{
		auto CommandRecast = static_cast<const LandscapeSystemCommand_Responce*>(Command);
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