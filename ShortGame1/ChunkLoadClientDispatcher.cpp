#include "stdafx.h"
#include "ChunkLoadClientDispatcher.h"
#include <LandscapeSystem_Command.h>

#include <Mixin_Movable.h>

namespace GEM
{


	void ChunkLoadClientDispatcher::FormRequest(GameSim::ClientCommandDispatcher * MessageDispatcher)
	{
		auto& NewlyVisibleChunks = m_chunkController.getGlobalyNewlyVisibleChunks();

		if (NewlyVisibleChunks.size() == 0) { return; }

		GameSim::LandscapeSystemCommand_RequestChunks RequestCommand;

		for (auto& visibleChunk : NewlyVisibleChunks)
		{
			RequestCommand.requests.emplace_back();
			RequestCommand.requests.back().x = visibleChunk.x;
			RequestCommand.requests.back().z = visibleChunk.z;

			if (m_chunkLoader.isChunkAvaliable(visibleChunk.x, visibleChunk.z))
			{
				if (m_chunks.getChunk(visibleChunk.x, visibleChunk.z) != nullptr) { continue; }

				auto NewChunk = m_chunks.addChunk(visibleChunk.x, visibleChunk.z, [&](GameSim::LandscapeChunk* ch) {
					m_chunkLoader.LoadChunkIn(visibleChunk.x, visibleChunk.z, ch); });
				std::get<ChunkConfirmationEnsurance>(NewChunk->AdditionalData).isConfirmed = false;

				RequestCommand.requests.back().version = NewChunk->chunk.getVersion();
			}
			else
			{
				RequestCommand.requests.back().version = 0;
			}
		}
		if (NewlyVisibleChunks.size() != 0) { m_chunks.generateMeshesForUnpreparedChunks(); }
		MessageDispatcher->InsertPerformedCommand(std::make_unique<GameSim::LandscapeSystemCommand_RequestChunks>(RequestCommand));
	}


	void ChunkLoadClientDispatcher::Process(GameSim::ClientCommandDispatcher* MessageDispatcher)
	{
		m_chunkController.ProcessChunks();
		FormRequest(MessageDispatcher);
	}
}
