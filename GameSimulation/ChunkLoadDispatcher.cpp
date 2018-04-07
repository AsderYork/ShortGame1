#include "ChunkLoadDispatcher.h"

namespace GEM::GameSim
{
	void GEM::GameSim::ChunkLoadDispatcher_Server::ProcessChunksRequirements()
	{
		auto NewlyVisibleChunks = m_chunkController.getGlobalyNewlyVisibleChunks();

		for (auto& visibleChunk : NewlyVisibleChunks)
		{
			if (m_chunkLoader.isChunkAvaliable(visibleChunk.x, visibleChunk.z))
			{
				m_chunks.emplace_back();
				m_chunkLoader.LoadChunkIn(visibleChunk.x, visibleChunk.z, &(m_chunks.back()));
			}
			else
			{
				m_chunks.emplace_back();
				m_chunkGenerator.FillChunkIn(visibleChunk.x, visibleChunk.z, &(m_chunks.back()));
			}
		}

		auto NoLongerVisibleChunks = m_chunkController.getGlobalyNoLongerVisibleChunks();
		for (auto& outvisibleChunk : NoLongerVisibleChunks)
		{
			for (auto it = m_chunks.begin(); it != m_chunks.end(); it++)
			{
				auto[x, z] = it->getPosition();
				if (x == outvisibleChunk.x && z == outvisibleChunk.z)
				{
					m_chunks.erase(it);
					break;
				}
			}
		}
	}
}
