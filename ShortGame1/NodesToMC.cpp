#include "stdafx.h"
#include "NodesToMC.h"

#include <vector>

namespace GEM
{
	NodesToMCGeneratorController::NodesToMCGeneratorController(ChunkLoader<NodeChunk>* chunkLoader) :
		m_chunkLoader(chunkLoader)
	{

	}

	void NodesToMCGeneratorController::GenerateFromScratch(int x, int y, Ogre_Service* ogreService)
	{
		/**
		So we have a position, for which chunk must be generated. What next?
		The chunk contains allmost all the nodes, to build a mesh for it. Except the right-most and front-most nodes. And the one, that right on the edge of
		these three chunks. So 4 chunks in total. One current and 3 nearby.


		*/
		//Get all needed chunks
		auto chunkCentre = m_chunkLoader->getChunk(x, y);

		auto chunkFront = m_chunkLoader->getChunk(x, y + 1);
		auto chunkRight = m_chunkLoader->getChunk(x + 1, y);
		auto chunkFrontRight = m_chunkLoader->getChunk(x + 1, y + 1);

		//Emplace generator in a vector
		//m_chunkGenerators.push_back();
		//int ID = m_chunkGenerators.size() - 1;

		m_listChunkUnit.emplace_back();
		m_listChunkUnit.back().Generator = std::make_unique<NodeToMCGeneratorNaive>(chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT, x, y);
		m_listChunkUnit.back().Mesher = std::make_unique<MCToMesh>(ogreService, m_listChunkUnit.back().Generator.get(), x*CHUNK_SIZE, y*CHUNK_SIZE, 1);

		m_listChunkUnit.back().Generator->Generate();
		m_listChunkUnit.back().Mesher->GenerateMesh();
	}

	void NodesToMCGeneratorController::PrepareChunk(int x, int z, Ogre_Service * ogreService)
	{
		//Check that asked chunk is not allready loaded somehow(PREPRARING, PREPARED, SHOWN)
		for (auto& Unit : m_ChunkUnits)
		{
			if ((Unit.x == x) && (Unit.z == z))
			{//Then the chunk is allready not unloaded
				return;
			}
		}

		//If that is not the case, then populate the m_chunks with it!
		auto chunkCentre = m_chunkLoader->getChunk(x, z);
		auto chunkFront = m_chunkLoader->getChunk(x, z + 1);
		auto chunkRight = m_chunkLoader->getChunk(x + 1, z);
		auto chunkFrontRight = m_chunkLoader->getChunk(x + 1, z + 1);

		auto it = m_chunks.emplace(m_chunks.end());
		it->Generator = std::make_unique<NodeToMCGeneratorNaive>(chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT, x, z);
		it->Mesher = std::make_unique<MCToMesh>(ogreService, it->Generator.get(), x*CHUNK_SIZE, z*CHUNK_SIZE, 1);

		//Add work
		m_workQueue.push_back(it);

		//Show that it's now not unloaded
		m_ChunkUnits.emplace_back(x, z, it);
	}

	void NodesToMCGeneratorController::UnloadChunk(int x, int z)
	{
		for (auto& it = m_ChunkUnits.begin(); it != m_ChunkUnits.end(); it++)
		{
			if ((it->x == x) && (it->z == z))
			{//Then the chunk is allready not unloaded
				auto ChunkIt = it->ChunkListIter;
				m_ChunkUnits.erase(it);

				//Then the real fun
				bool Ex = false;
				if (it->ChunkListIter->isActive.compare_exchange_strong(Ex, true))
				{//If noone is using it, mark it for delition
					it->ChunkListIter->isActive.store(false);
					//In that case, we just have to remove this chunk, becouse for now we're own it.
					//But it still might go throuh 
				}
				it->ChunkListIter->isActive.store(false);
				return;
			}
		}
	}

	void NodesToMCGeneratorController::UnloadAllChunks()
	{		
		m_listChunkUnit.clear();
	}

	void NodesToMCGeneratorController::UpdateChunk(int x, int y, Ogre_Service * ogreService)
	{
		//Check if chunk is loaded
		for (auto& Unit : m_listChunkUnit)
		{
			if ((Unit.Generator->getChunkX() == x) && (Unit.Generator->getChunkZ() == y))
			{
				//Update Chunk
				Unit.Generator->Update();
				Unit.Mesher->GenerateMesh();

				break;
			}
		}
	}

	void NodesToMCGeneratorController::ChangeNode(int ChunkX, int ChunkZ, int X, int Y, int Z)
	{
		//Check if chunk is loaded
		for (auto& Unit : m_listChunkUnit)
		{
			if ((Unit.Generator->getChunkX() == ChunkX) && (Unit.Generator->getChunkZ() == ChunkZ))
			{
				//Update Chunk
				Unit.Generator->ChangeNode(X, Y, Z);
				break;
			}
		}
	}

	void NodesToMCGeneratorController::ChunkLoaderThredFunc()
	{
		std::list<ChunkCore>::iterator WorkIter;
		bool WorkFound = false;

		while (m_ContinueThread.load())
		{
			m_workQueueMutex.lock();
			//If WorkQueue is not empty, grab one task and RUN!
			if (m_workQueue.size() != 0)
			{
				WorkIter = m_workQueue.front();
				m_workQueue.pop_front();
				WorkFound = true;
			}
			m_workQueueMutex.unlock();			

			if (WorkFound)
			{
				WorkIter->Generator->Generate();
				WorkIter->isBuilt.store(true);
				//If it's allready useles
				bool Ex = false;
				//Check that noone wants to delete it right now. Becouse if someone does, iterator might be allready invalidated
				if (WorkIter->isInUse.compare_exchange_strong(Ex, true))
				{
					if (!WorkIter->isActive.load())
					{//Hope noone did traverse this list at this moment. Otherwise thay could run on to this chunk right when it's iterator became invalid
						m_chunks.erase(WorkIter);
					}
				}
				//
				WorkFound = false;
			}

		}
	}


}
