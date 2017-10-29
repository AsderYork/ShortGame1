#include "stdafx.h"
#include "NodesToMC.h"

#include <vector>
#include <chrono>

namespace GEM
{
	typedef std::chrono::duration<float> fsec;
	NodesToMCGeneratorController::NodesToMCGeneratorController(ChunkLoader<NodeChunk>* chunkLoader, Ogre_Service* ogreService) :
		m_chunkLoader(chunkLoader), m_ogreService(ogreService)
	{
		m_workerThread = std::thread(&NodesToMCGeneratorController::ChunkLoaderThredFunc, this);
	}
		

	void NodesToMCGeneratorController::PrepareChunk(int x, int z)
	{
		//Check that asked chunk is not allready loaded somehow(PREPRARING, PREPARED, SHOWN)
		for (auto& Unit : m_ChunkUnits)
		{
			if ((Unit.x == x) && (Unit.z == z))
			{//Then the chunk is allready not unloaded
				return;
			}
		}

		auto it = m_chunks.emplace(m_chunks.end(), x, z);//Create new chunkCore

		//Add work
		m_workQueueMutex.lock();
		m_workQueue.push_back(it);
		m_workQueueMutex.unlock();

		//Show that it's now not unloaded
		m_ChunkUnits.emplace_back(x, z, it);
	}

	void NodesToMCGeneratorController::ShowChunk(int x, int z)
	{
		auto t0 = std::chrono::high_resolution_clock::now();

		//Check if it's not unloaded.
		for (auto& Unit : m_ChunkUnits)
		{
			if ((Unit.x == x) && (Unit.z == z))
			{
				//Wait while chunk is built if its still isn't
				while (!Unit.ChunkListIter->isBuilt.load(std::memory_order_acquire)) {};
				Unit.ChunkListIter->Mesher->GenerateMesh();
				auto t1 = std::chrono::high_resolution_clock::now();
				printf("Found:%f\n",std::chrono::duration_cast<fsec>(t1 - t0).count());
				return;

			}
		}//If we still here then this chunk wasnt loaded! Load it from this thread

		
	}

	void NodesToMCGeneratorController::UnloadChunk(int x, int z)
	{
		for (auto& it = m_ChunkUnits.begin(); it != m_ChunkUnits.end(); it++)
		{
			if ((it->x == x) && (it->z == z))
			{//Then the chunk is allready not unloaded
				auto ChunkIt = it->ChunkListIter;
				//Remove it from the sercher's list
				m_ChunkUnits.erase(it);

				ChunkIt->Mesher.reset();//Ogre doesn't like, when object dessapear in the middle of a frame. So remove it before all parallel things
				m_workQueueMutex.lock();
				m_workQueue.push_back(ChunkIt);
				m_workQueueMutex.unlock();
				return;
			}
		}
	}

	void NodesToMCGeneratorController::UpdateChunk(int x, int z)
	{
		//Check if chunk is loaded
		for (auto& Unit : m_ChunkUnits)
		{
			if ((Unit.x == x) && (Unit.z == z))
			{
				//Wait while chunk is built if its still isn't
				while (!Unit.ChunkListIter->isBuilt.load()) {};
				//Update Chunk
				Unit.ChunkListIter->Generator->Update();
				Unit.ChunkListIter->Mesher->GenerateMesh();

				break;
			}
		}
	}

	void NodesToMCGeneratorController::ChangeNode(int ChunkX, int ChunkZ, int X, int Y, int Z)
	{
		for (auto& Unit : m_ChunkUnits)
		{
			if ((Unit.x == ChunkX) && (Unit.z == ChunkZ))
			{
				//Wait while chunk is built if its still isn't
				while (!Unit.ChunkListIter->isBuilt.load()) {};
				Unit.ChunkListIter->Generator->ChangeNode(X, Y, Z);
			}
		}
	
	}

	void NodesToMCGeneratorController::ShutDown()
	{
		m_ContinueThread.store(false);
		m_workerThread.join();
		//All the work is done. No more parallelism
		m_workQueue.clear();
		m_ChunkUnits.clear();
		m_chunks.clear();
	}

	NodesToMCGeneratorController::~NodesToMCGeneratorController()
	{
		//If it still wan't shutDown
		if (m_workerThread.joinable())
		{
			ShutDown();
		}


	}

	void NodesToMCGeneratorController::ChunkLoaderThredFunc()
	{
		std::list<ChunkCore>::iterator WorkIter;
		volatile bool WorkFound = false;

		while (m_ContinueThread.load(std::memory_order_acquire))
		{
			WorkFound = false;
			//If WorkQueue is not empty, grab one task and RUN!
			m_workQueueMutex.lock();
			if (m_workQueue.size() != 0)
			{
				WorkIter = m_workQueue.front();
				m_workQueue.pop_front();
				WorkFound = true;
			}
			m_workQueueMutex.unlock();

			if (WorkFound)
			{
				if (!WorkIter->isBuilt.load(std::memory_order_acquire))
				{
					//Get chunk nodes
					auto chunkCentre = m_chunkLoader->getChunk(WorkIter->x, WorkIter->z);
					auto chunkFront = m_chunkLoader->getChunk(WorkIter->x, WorkIter->z + 1);
					auto chunkRight = m_chunkLoader->getChunk(WorkIter->x + 1, WorkIter->z);
					auto chunkFrontRight = m_chunkLoader->getChunk(WorkIter->x + 1, WorkIter->z + 1);


					WorkIter->Generator = std::make_unique<NodeToMCGeneratorNaive>(chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT, WorkIter->x, WorkIter->z);
					WorkIter->Mesher = std::make_unique<MCToMesh>(m_ogreService, WorkIter->Generator.get(), WorkIter->x*CHUNK_SIZE, WorkIter->z*CHUNK_SIZE, 1);

					WorkIter->Generator->Generate();
					//If it's allready useles
					bool Ex = false;
					WorkIter->isBuilt.store(true, std::memory_order_release);
				}
				else 
				{//Then it's here to be deleatad
					m_chunks.erase(WorkIter);
				}
				
		
			}

		}
	}


}
