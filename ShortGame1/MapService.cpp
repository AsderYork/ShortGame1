#include "stdafx.h"
#include "MapService.h"
#include <chrono>
#include <math.h>
#include <OGRE\OgreCamera.h>
#include <algorithm>
#include <chrono>

namespace GEM
{

	typedef std::chrono::duration<float> fsec;
	void MapService::ProcessCameraMovement()
	{
		auto t0 = std::chrono::high_resolution_clock::now();
		auto CameraPos = m_ogreService->getCamera()->getPosition();
		auto CameraChunk = getChunk(CameraPos.x, CameraPos.y, CameraPos.z);
		

		std::list<std::pair<int, int>> SurvivedShownChunks;
		std::list<std::pair<int, int>> SurvivedPrepareChunks;

		//Iterate through all chunks, that must be prepared
		for (int x = CameraChunk.first - m_prepareDistance; x <= CameraChunk.first + m_prepareDistance; x++)
		{
			for (int y = CameraChunk.second - m_prepareDistance; y <= CameraChunk.second + m_prepareDistance; y++)
			{
				//Check if it's prepared
				auto& it = std::find(m_preparingChunks.begin(), m_preparingChunks.end(), std::make_pair(x, y));
				//Check if it's allready preparing
				if (it != m_preparingChunks.end())
				{//If it is, then move it to survivers
					SurvivedPrepareChunks.push_back(*it);//If it is, then move it to survived chunks
					m_preparingChunks.erase(it);
				}
				else
				{//If it wasn't prepared before. Start preparing and move it to survivers
					m_generator.PrepareChunk(x, y, m_ogreService);
					SurvivedPrepareChunks.push_back(std::make_pair(x, y));
				}

				//If it's also a chunk to be shown, then process it more
				if ((abs(CameraChunk.first - x) < m_drawDistance) && (abs(CameraChunk.second - y) < m_drawDistance))
				{
					auto& it = std::find(m_shownChunks.begin(), m_shownChunks.end(), std::make_pair(x, y));
					if (it != m_shownChunks.end())//Check if it's shown
					{
						SurvivedShownChunks.push_back(*it);//If it is, then move it to survived chunks
						m_shownChunks.erase(it);
					}
					else {//If it's not, then force it to be shown, and move it to survivers
						m_generator.ShowChunk(x, y, m_ogreService);
						SurvivedShownChunks.push_back(std::make_pair(x, y));
					}
				}				
			}
		}

		auto ts = std::chrono::high_resolution_clock::now();

		//All the chunks that wasn't moved from Preparing CHunks must be cleared;
		for (auto& ChunkForUnload : m_preparingChunks)
		{
			m_generator.UnloadChunk(ChunkForUnload.first, ChunkForUnload.second);
		}
		m_shownChunks = std::move(SurvivedShownChunks);//Chunks from shown, that havn't been moved, might just gone to Prepared, so they should stay
		m_preparingChunks = std::move(SurvivedPrepareChunks);


		auto t1 = std::chrono::high_resolution_clock::now();
		printf("Search:%f\n", std::chrono::duration_cast<fsec>(ts - t0).count());
		printf("Overall:%f\n", std::chrono::duration_cast<fsec>(t1 - t0).count());

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
		auto CameraPos = m_ogreService->getCamera()->getPosition();
		auto CameraChunk = getChunk(CameraPos.x, CameraPos.y, CameraPos.z);

		m_currentChunk = CameraChunk;
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
		m_generator.ShutDown();
	}

	Service::ActionResult MapService::preFrame(double timeDelta)
	{
		auto CameraPos = m_ogreService->getCamera()->getPosition();
		auto CameraChunk = getChunk(CameraPos.x, CameraPos.y, CameraPos.z);
		if ((m_currentChunk.first != CameraChunk.first) || (m_currentChunk.second != CameraChunk.second))
		{
			ProcessCameraMovement();
			m_currentChunk = CameraChunk;
		}
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
