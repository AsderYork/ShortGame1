#include "stdafx.h"
#include "MapService.h"
#include <chrono>

namespace GEM
{
	void MapService::SetIndividualNode(int NodeX, int NodeY, int NodeZ, unsigned char value)
	{
		if((NodeY>CHUNK_HEIGHT) || (NodeY <0)){	return;	}

		//Find a chunk cordinate, where this node should be
		int ChunkX = (NodeX) / CHUNK_SIZE;
		if (NodeX < 0) { ChunkX--; }
		int ChunkZ = (NodeZ) / CHUNK_SIZE;
		if (ChunkZ < 0) { ChunkZ--; }

		int XInChunk = NodeX - ChunkX*(CHUNK_SIZE);
		int ZInChunk = NodeZ - ChunkZ*(CHUNK_SIZE);

		auto Chunk = m_loader.getChunk(ChunkX, ChunkZ);		

		Chunk->NodeMap[XInChunk][NodeY][ZInChunk].Value = value;
		Chunk->NodeMap[XInChunk][NodeY][ZInChunk].isChanged = true;

		//Check for edge cases
		if (XInChunk == 0)
		{
			m_generator.ChangeNode(ChunkX-1, ChunkZ, XInChunk+32, NodeY, ZInChunk);
			m_changedChunks.push_back(std::make_pair(ChunkX-1, ChunkZ));
			if (ZInChunk == 0)
			{
				m_changedChunks.push_back(std::make_pair(ChunkX-1, ChunkZ-1));
				m_generator.ChangeNode(ChunkX - 1, ChunkZ - 1, XInChunk+32, NodeY, ZInChunk+32);
			}
		}
		if (ZInChunk == 0)
		{
			m_changedChunks.push_back(std::make_pair(ChunkX, ChunkZ-1));
			m_generator.ChangeNode(ChunkX, ChunkZ - 1, XInChunk, NodeY, ZInChunk+32);
		}

		m_generator.ChangeNode(ChunkX, ChunkZ, XInChunk, NodeY, ZInChunk);
		m_changedChunks.push_back(std::make_pair(ChunkX, ChunkZ));

	}
	unsigned char MapService::getNodeVal(int NodeX, int NodeY, int NodeZ)
	{
		//Find a chunk cordinate, where this node should be
		int ChunkX = (NodeX - 1) / CHUNK_SIZE;
		if (NodeX < 0) { ChunkX--; }
		int ChunkZ = (NodeZ - 1) / CHUNK_SIZE;
		if (ChunkZ < 0) { ChunkZ--; }

		int XInChunk = NodeX - ChunkX*CHUNK_SIZE;
		int ZInChunk = NodeZ - ChunkZ*CHUNK_SIZE;

		auto Chunk = m_loader.getChunk(ChunkX, ChunkZ);		

		return Chunk->NodeMap[XInChunk][NodeY][ZInChunk].Value;
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
