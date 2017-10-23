#include "stdafx.h"
#include "MapService.h"
#include <chrono>
#include <math.h>
#include <OGRE\OgreCamera.h>
#include <algorithm>

namespace GEM
{
	void MapService::ProcessCameraMovement()
	{
		auto CameraPos = m_ogreService->getCamera()->getPosition();
		auto CameraChunk = getChunk(CameraPos.x, CameraPos.y, CameraPos.z);
		

		std::list<std::pair<int, int>> SurvivedChunks;
		for (int x = CameraChunk.first - m_drawDistance; x <= CameraChunk.first + m_drawDistance; x++)
		{
			for (int y = CameraChunk.second - m_drawDistance; y <= CameraChunk.second + m_drawDistance; y++)
			{
				auto& it = std::find(m_activeChunks.begin(), m_activeChunks.end(), std::make_pair(x, y));
				if (it != m_activeChunks.end())
				{
					SurvivedChunks.push_back(*it);
					m_activeChunks.erase(it);
				}
				else{//If there wasn't required chunk
					m_generator.GenerateFromScratch(x, y, m_ogreService);
					SurvivedChunks.push_back(std::make_pair(x, y));
				}
			}
		}

		for (auto& ChunkForUnload : m_activeChunks)
		{
			m_generator.UnloadChunk(ChunkForUnload.first, ChunkForUnload.second);
		}
		m_activeChunks.clear();
		m_activeChunks.splice(m_activeChunks.begin(), SurvivedChunks);
		auto Si = m_activeChunks.size();

	}
	std::pair<int, int> MapService::getChunk(float x, float y, float z)
	{
		int retX = 0;
		if (x<0)
		{
			retX = ((trunc(x)) / CHUNK_SIZE) - 1;}
		else
		{
			retX = trunc(x) / CHUNK_SIZE;}

		int retY = 0;
		if (z<0)
		{
			retY = ((trunc(z)) / CHUNK_SIZE) - 1;
		}
		else
		{
			retY = trunc(z) / CHUNK_SIZE;
		}

		return std::make_pair(retX, retY);
	}
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
		ProcessCameraMovement();		
		//m_generator.GenerateFromScratch(-1, -1, m_ogreService);
		//m_generator.GenerateFromScratch(0, -1, m_ogreService);
		//m_generator.GenerateFromScratch(1, -1, m_ogreService);

		//m_generator.GenerateFromScratch(-1, 0, m_ogreService);
		//m_generator.GenerateFromScratch(0, 0, m_ogreService);
		//m_generator.GenerateFromScratch(1, 0, m_ogreService);

		//m_generator.GenerateFromScratch(-1, 1, m_ogreService);
		//m_generator.GenerateFromScratch(0, 1, m_ogreService);
		//m_generator.GenerateFromScratch(1, 1, m_ogreService);


		return ActionResult();
	}

	void GEM::MapService::shutdown()
	{
		m_generator.UnloadAllChunks();
	}

	Service::ActionResult MapService::preFrame(double timeDelta)
	{
		ProcessCameraMovement();
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
