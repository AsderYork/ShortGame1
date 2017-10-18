#include "stdafx.h"
#include "MapService.h"
#include <chrono>

namespace GEM
{
	void MapService::SetIndividualNode(int NodeX, int NodeY, int NodeZ, unsigned char value)
	{
		//Find a chunk cordinate, where this node should be
		int ChunkX = (NodeX - 1) / CHUNK_SIZE;
		if (NodeX < 0) { ChunkX--; }
		int ChunkZ = (NodeZ - 1) / CHUNK_SIZE;
		if (ChunkZ < 0) { ChunkZ--; }

		int XInChunk = NodeX - ChunkX*CHUNK_SIZE;
		int ZInChunk = NodeZ - ChunkZ*CHUNK_SIZE;

		auto Chunk = m_loader.getChunk(ChunkX, ChunkZ);

		m_changedChunks.push_back(std::make_pair(ChunkX, ChunkZ));

		Chunk->NodeMap[XInChunk][NodeY][ZInChunk].Value = value;


	}
	Service::ActionResult MapService::initialize()
	{		

		auto start = std::chrono::steady_clock::now();
		m_generator.GenerateFromScratch(0, 0, m_ogreService);
		m_generator.GenerateFromScratch(1, 0, m_ogreService);


		return ActionResult();
	}

	void GEM::MapService::shutdown()
	{
	}

	Service::ActionResult MapService::preFrame(double timeDelta)
	{
		//Update changed chunks. Ignore chunks, that are not actually shown
		for (auto& ChunlPos : m_changedChunks)
		{
			m_generator.UpdateChunk(ChunlPos.first, ChunlPos.second, m_ogreService);

		}

		//Clear changedChunks
		m_changedChunks.clear();
		return ActionResult();
	}

	Service::ActionResult MapService::frame(double timeDelta)
	{
		return ActionResult();
	}

	Service::ActionResult MapService::postFrame(double timeDelta)
	{
		return ActionResult();
	}
}
