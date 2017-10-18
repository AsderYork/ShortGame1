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
		m_generatedChunks.push_back(std::make_unique<NodesToMCGenerator>(chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT, x, y));
		int ID = m_generatedChunks.size() - 1;


		m_generatedChunks[ID]->Generate();
		//Generators and MCToMEeshes creates simultaniously so they share the same id
		m_mcToMeshes.push_back(std::make_unique<MCToMesh>(ogreService, m_generatedChunks[ID].get(), x*CHUNK_SIZE, y*CHUNK_SIZE, 1));
		m_mcToMeshes[ID]->GenerateMesh();
	}

	void NodesToMCGeneratorController::UpdateChunk(int x, int y, Ogre_Service * ogreService)
	{
		//Check if chunk is loaded
		for (int i = 0; i< m_generatedChunks.size(); i++)
		{
			if ((m_generatedChunks[i]->getChunkX() == x) && (m_generatedChunks[i]->getChunkZ() == y))
			{
				//Update Chunk
				m_generatedChunks[i]->Generate();
				m_mcToMeshes[i]->GenerateMesh();

				break;
			}
		}
	}


}
