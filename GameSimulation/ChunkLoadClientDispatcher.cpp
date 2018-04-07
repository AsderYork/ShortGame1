#include "ChunkLoadClientDispatcher.h"
#include "LandscapeSystem_Command.h"

#include "Mixin_Movable.h"

namespace GEM::GameSim
{

	void ChunkLoadClientDispatcher::FormRequest(ClientCommandDispatcher * MessageDispatcher)
	{
		auto& NewlyVisibleChunks = m_chunkController.getGlobalyNewlyVisibleChunks();

		LandscapeSystemCommand_RequestChunks RequestCommand;

		for (auto& visibleChunk : NewlyVisibleChunks)
		{
			RequestCommand.requests.emplace_back();
			RequestCommand.requests.back().x = visibleChunk.x;
			RequestCommand.requests.back().z = visibleChunk.z;

			if (m_chunkLoader.isChunkAvaliable(visibleChunk.x, visibleChunk.z))
			{
				m_chunks.emplace_back();
				m_chunkLoader.LoadChunkIn(visibleChunk.x, visibleChunk.z, &(m_chunks.back()));

				RequestCommand.requests.back().version = m_chunks.back().getVersion();
			}
			else
			{
				RequestCommand.requests.back().version = 0;
			}
		}

		MessageDispatcher->InsertPerformedCommand(std::make_unique<LandscapeSystemCommand_RequestChunks>(RequestCommand));
	}
}
