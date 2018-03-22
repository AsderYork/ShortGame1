#include "LandscapeChunkController.h"

namespace GEM::GameSim
{
	uint64_t LandscapeChunkController::LoaderType::LastLoaderUniqueID = 0;

void LandscapeChunkController::ProcessChunks()
{
	std::vector<ChunkPos> ThisTickVisibleChunks;
	//Pass 1. Find chunks, that are visible now and find newly visible chunks for evry loader
	for (auto& loader : m_loaders)
	{
		std::vector<ChunkPos> VisibleChunksForLoader;
		auto LoaderPos = ChunkPos::getChunkFromPoint(loader.posFunc());

		for (auto x = LoaderPos.x - m_loadRadius; x < LoaderPos.x + m_loadRadius + 1; x++)
		{
			for (auto z = LoaderPos.z - m_loadRadius; z < LoaderPos.z + m_loadRadius + 1; z++)
			{
				VisibleChunksForLoader.emplace_back(x, z);

				auto ChunkIter = std::upper_bound(ThisTickVisibleChunks.begin(), ThisTickVisibleChunks.end(), ChunkPos(x, z), [](const ChunkPos& LP, const ChunkPos& CP) {return LP <= CP; });
				if (ChunkIter == ThisTickVisibleChunks.end())
				{
					ThisTickVisibleChunks.emplace_back(x, z);
				}
				else if ((*ChunkIter) != ChunkPos(x, z))
				{
					ThisTickVisibleChunks.emplace(ChunkIter, x, z);
				}
				else
				{
					continue;
				}
			}

		}
		//Calculate chunks, that are new for this loader
		loader.newlyVisibleChunks = FindDifferences(VisibleChunksForLoader, loader.visbleChunks).first;
		loader.visbleChunks = VisibleChunksForLoader;
	}
	//Pass two - find globaly new chunks and chunks that are no longer visible

	auto[NewChunks, NoLongerVisibleChunks] = FindDifferences(ThisTickVisibleChunks, m_globalyVisibleChunks);
	m_noLongerVisibleChunks = NoLongerVisibleChunks;
	m_newlyGlobalyVisibleChunks = NewChunks;
	m_globalyVisibleChunks = ThisTickVisibleChunks;
}




};