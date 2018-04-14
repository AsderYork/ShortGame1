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

	LandscapeChunk * ChunkLoadServerDispatcher::getChunk(int x, int z)
	{
		auto SearchLambda = [](const LandscapeChunk& chunk, std::pair<int, int> cord) {
			return chunk.getPosition() < cord;
		};

		auto SearchResult = std::lower_bound(m_chunks.begin(), m_chunks.end(), std::make_pair(x, z), SearchLambda);
		if (SearchResult == m_chunks.end())
		{
			return nullptr;
		}
		else if (SearchResult->getPosition() != std::make_pair(x, z))
		{
			return nullptr;
		}

		return &(*SearchResult);
	}

	LandscapeChunk * ChunkLoadServerDispatcher::addNewChunk(int x, int z)
	{
		auto SearchLambda = [](const LandscapeChunk& chunk, std::pair<int, int> cord) {
			return chunk.getPosition() < cord;
		};

		auto SearchResult = std::lower_bound(m_chunks.begin(), m_chunks.end(), std::make_pair(x, z), SearchLambda);
		if (SearchResult == m_chunks.end())
		{
			SearchResult = m_chunks.emplace(SearchResult);
		}
		else if (SearchResult->getPosition() != std::make_pair(x, z))
		{
			SearchResult = m_chunks.emplace(SearchResult);
		}

		return &(*SearchResult);
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
				auto Chunkptr = addNewChunk(visibleChunk.x, visibleChunk.z);
				m_chunkLoader.LoadChunkIn(visibleChunk.x, visibleChunk.z, Chunkptr);
			}
			else
			{
				auto Chunkptr = addNewChunk(visibleChunk.x, visibleChunk.z);
				m_chunkGenerator.FillChunkIn(visibleChunk.x, visibleChunk.z, Chunkptr);
				m_chunkLoader.SaveChunk(Chunkptr);
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
