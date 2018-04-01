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
			for (int i = 0; i < m_chunks.size(); i++)
			{
				auto[x, z] = m_chunks[i].getPosition();
				if (x == outvisibleChunk.x && z == outvisibleChunk.z)
				{
					m_chunkLoader.SaveChunk(&(m_chunks[i]));
					m_chunks.erase(m_chunks.begin() + i);
					break;
				}
			}
		}
	}
}
