#include "stdafx.h"
#include "NodesToMC.h"
#include "NodesToMCGenerator.h"
#include "MCToMesh.h"

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

		NodesToMCGenerator Generator(chunkCentre, chunkRight, chunkFront, chunkFrontRight, CHUNK_SIZE, CHUNK_HEIGHT);

		Generator.Generate();
		MCToMesh ToMesh(ogreService);
		
		ToMesh.GenerateMesh(Generator, x*CHUNK_SIZE, y*CHUNK_SIZE, 1);
	}


}
