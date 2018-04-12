#include "ChunkLoadServerDispatcher.h"
#include <chrono>

namespace GEM::GameSim
{
	void ChunkLoadServerDispatcher::PerformOccasionalSave()
	{
		static auto LastSaveTime = std::chrono::system_clock::now();

		auto CurrTime = std::chrono::system_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(CurrTime - LastSaveTime).count() > 30)
		{
			LastSaveTime = CurrTime;
			m_chunkLoader.SaveMagistral();
		}

	}

	void ChunkLoadServerDispatcher::ProcessChunks()
	{
		m_chunkController.ProcessChunks();
		PerformOccasionalSave();

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
				m_chunkLoader.SaveChunk(&(m_chunks.back()));
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
					m_chunkLoader.SaveChunk(&(*it));
					m_chunks.erase(it);
					break;
				}
			}
		}
	}


	void ChunkLoadServerDispatcher::Start()
	{
		m_chunkLoader.Start("./Map");
	}
}
