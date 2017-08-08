#include "stdafx.h"
#include "VoxelMap.h"

namespace GEM
{

	VoxelMap::VoxelMap()
	{
		m_chunkLoader = ChunkLoader::GetOrCreateChunkLoader();
		m_chunkMap.resize(m_loadRadius*m_loadRadius);

		m_cubesCalculator.FillNodeList(m_chunkLoader->getChunk(intpos2(0,0)).get());
	}

	void VoxelMap::SetPlayerPos(int x, int y)
	{
		auto playerChunk = fromWorldPosToChunkPos(x,y);
		m_playerChunk = playerChunk;
		UpdateChunkListForCurrentPosition(playerChunk);
	}

	void VoxelMap::UpdateChunkListForCurrentPosition(intpos2 CurrentPosition)
	{
		std::vector<std::shared_ptr<Chunk>> newChunks;
		for (int x = CurrentPosition.x - m_loadRadius; x < CurrentPosition.x + m_loadRadius; x++)
		{
			for (int y = CurrentPosition.y - m_loadRadius; y < CurrentPosition.y + m_loadRadius; y++)
			{
				newChunks.push_back(m_chunkLoader->getChunk(intpos2(x, y)));
			}
		}
		m_chunkMap.swap(newChunks);
	}

	void VoxelMap::ResizeChunkMap(int NewRadius)
	{
		if (NewRadius > m_loadRadius)
		{//Then we can just add space in a chunk map!
			int IncresedSize = (NewRadius * 2) * (NewRadius * 2);
			while (m_chunkMap.size() < IncresedSize)
			{
				m_chunkMap.push_back(nullptr);
			}
		}
		else if (NewRadius == m_loadRadius) { ; }//Do nothing
		else {
			std::vector<std::shared_ptr<Chunk>> newChunkMap;
			for (auto& chunk : m_chunkMap)
			{
				if ((abs(chunk->ChunkPos.x - m_playerChunk.x) <= NewRadius) && (abs(chunk->ChunkPos.y - m_playerChunk.y) <= NewRadius))
				{//Chunk Stays;
					newChunkMap.push_back(std::move(chunk));
				}
			}
			//Al the chunks, that havn't been move, will be deleted when newChunkMap get out of scope.
			m_chunkMap.swap(newChunkMap);
		}

		m_loadRadius = NewRadius;		
	}

	MarchingCubiesMeshCalc * VoxelMap::getMeshCalc()
	{
		return &m_cubesCalculator;
	}

	VoxelMap::~VoxelMap()
	{
		m_chunkMap.clear();
	}


}