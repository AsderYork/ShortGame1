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
				m_chunks.addChunk(visibleChunk.x, visibleChunk.z,
					[&](LandscapeChunk* chptr) {m_chunkLoader.LoadChunkIn(visibleChunk.x, visibleChunk.z, chptr); }
				);				
			}
			else
			{
				m_chunks.addChunk(visibleChunk.x, visibleChunk.z,
					[&](LandscapeChunk* chptr)
					{
						m_chunkGenerator.FillChunkIn(visibleChunk.x, visibleChunk.z, chptr);
						m_chunkLoader.SaveChunk(chptr);
					});
			}
		}

		auto NoLongerVisibleChunks = m_chunkController.getGlobalyNoLongerVisibleChunks();
		for (auto& outvisibleChunk : NoLongerVisibleChunks)
		{
			m_chunks.RemoveChunk(outvisibleChunk.x, outvisibleChunk.z, [&](LandscapeChunk* chunk) {m_chunkLoader.SaveChunk(chunk); });
		}
	}


	void ChunkLoadServerDispatcher::Start()
	{
		m_chunkLoader.Start("./Map");
	}
}
