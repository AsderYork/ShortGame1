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

	void ChunkLoadServerDispatcher::getOneChunk(int x, int z)
	{
		if (m_chunkLoader.isChunkAvaliable(x, z))
		{
			m_chunks.addChunk(x, z,	[&](LandscapeChunk* chptr) {m_chunkLoader.LoadChunkIn(x, z, chptr); });
		}
		else
		{
			m_chunks.addChunk(x,z,	[&](LandscapeChunk* chptr){	m_chunkGenerator.FillChunkIn(x, z, chptr);	m_chunkLoader.SaveChunk(chptr);	});
		}
	}
	
	void ChunkLoadServerDispatcher::ForceChunkCreation(int x, int z)
	{
		getOneChunk(x, z);
		getOneChunk(x + 1, z);
		getOneChunk(x, z + 1);
		getOneChunk(x + 1, z + 1);
		m_chunks.generateMeshesForUnpreparedChunks();
	}

	void ChunkLoadServerDispatcher::ProcessChunks()
	{
		m_chunkController.ProcessChunks();
		PerformOccasionalSave();

		auto NewlyVisibleChunks = m_chunkController.getGlobalyNewlyVisibleChunks();

		for (auto& visibleChunk : NewlyVisibleChunks)
		{
			getOneChunk(visibleChunk.x, visibleChunk.z);
		}

		if (NewlyVisibleChunks.size() > 0)
		{
			m_chunks.generateMeshesForUnpreparedChunks();
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
