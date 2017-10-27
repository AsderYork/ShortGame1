#include "stdafx.h"
#include "NodesToMC.h"

#include <vector>
#include <chrono>

namespace GEM
{
	typedef std::chrono::duration<float> fsec;
	NodesToMCGeneratorController::NodesToMCGeneratorController(ChunkLoader<NodeChunk>* chunkLoader) :
		m_chunkLoader(chunkLoader)
	{
		m_workerThread = std::thread(&NodesToMCGeneratorController::ChunkLoaderThredFunc, this);
	}
		

	void NodesToMCGeneratorController::PrepareChunk(int x, int z, Ogre_Service * ogreService)
	{
		auto t0 = std::chrono::high_resolution_clock::now();
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
		auto t1 = std::chrono::high_resolution_clock::now();
		printf("Prepared:%f\n", std::chrono::duration_cast<fsec>(t1 - t0).count());
	}

	void NodesToMCGeneratorController::ShowChunk(int x, int z, Ogre_Service * ogreService)
	{
		auto t0 = std::chrono::high_resolution_clock::now();

		//Check if it's not unloaded.
		for (auto& Unit : m_ChunkUnits)
		{
			if ((Unit.x == x) && (Unit.z == z))
			{
				//Wait while chunk is built if its still isn't
				while (!Unit.ChunkListIter->isBuilt.load()) {};
				Unit.ChunkListIter->Mesher->GenerateMesh();
				auto t1 = std::chrono::high_resolution_clock::now();
				printf("Found:%f\n",std::chrono::duration_cast<fsec>(t1 - t0).count());
				return;

			}
		}//If we still here then this chunk wasnt loaded! Load it from this thread

		auto t1 = std::chrono::high_resolution_clock::now();
		printf("NotFound:%f\n", std::chrono::duration_cast<fsec>(t1 - t0).count());

		auto chunkCentre = m_chunkLoader->getChunk(x, z);
		auto chunkFront = m_chunkLoader->getChunk(x, z + 1);
		auto chunkRight = m_chunkLoader->getChunk(x + 1, z);
		auto chunkFrontRight = m_chunkLoader->getChunk(x + 1, z + 1);

		auto it = m_chunks.emplace(m_chunks.end());
		it->Generator = std::make_unique<NodeToMCGeneratorNaive>(chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT, x, z);
		it->Mesher = std::make_unique<MCToMesh>(ogreService, it->Generator.get(), x*CHUNK_SIZE, z*CHUNK_SIZE, 1);

		m_ChunkUnits.emplace_back(x, z, it);
		//Add work
		it->Generator->Generate();
		it->Mesher->GenerateMesh();
		it->isBuilt.store(true);

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

				ChunkIt->DelitionRoutineMutex.lock();
				{
					if (ChunkIt->isBuilt.load() == false)
					{//If it's still not built, then mark it to be destroyed
						ChunkIt->markedForDeletion.store(true);
					}
					else
					{//Otherwise it is safe to destroy it right now.
						ChunkIt->DelitionRoutineMutex.unlock();
						m_chunks.erase(ChunkIt);
					}
				}
				
				return;
			}
		}
	}

	void NodesToMCGeneratorController::UpdateChunk(int x, int z, Ogre_Service * ogreService)
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
		bool WorkFound = false;

		while (m_ContinueThread.load())
		{
			WorkFound = false;
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
				//If it's allready useles
				bool Ex = false;
				//Check if mutex is not grabbed right now.
				WorkIter->DelitionRoutineMutex.lock();				
				if (WorkIter->markedForDeletion.load())
				{//Then delete it
					WorkIter->DelitionRoutineMutex.unlock();
					m_chunks.erase(WorkIter);
					continue;//And skip flag set. There is no more flag to set!
				}

				WorkIter->isBuilt.store(true);

				WorkIter->DelitionRoutineMutex.unlock();				
			}

		}
	}


}
