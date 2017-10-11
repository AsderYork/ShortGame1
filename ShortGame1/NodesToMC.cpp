#include "stdafx.h"
#include "NodesToMC.h"

#include <vector>

namespace GEM
{
	NodesToMCGeneratorController::NodesToMCGeneratorController(ChunkLoader<NodeChunk>* chunkLoader) :
		m_chunkLoader(chunkLoader)
	{

	}

	void NodesToMCGeneratorController::GenerateFromScratch(int x, int y)
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

		
	}
	/*void NodesToMCGeneratorController::ProcessCube(std::vector<std::vector<std::vector<Cubie>>>& Cubies, int x, int y, int z)
	{
		Cubies[x][y][z].CalculateCubeval();
	}*/

	void NodesToMCGenerator::Generate()
	{
		for (int x = 0; x < DimXZ + 1; x++)
		{
			for (int y = 0; y < DimY; y++)
			{
				for (int z = 0; z < DimXZ + 1; z++)
				{
					GetNode(x, y, z);
				}
			}
		}
	}

}
