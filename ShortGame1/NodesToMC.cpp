#include "stdafx.h"
#include "NodesToMC.h"

#include <vector>

namespace GEM
{
	NodesToMCGenerator::NodesToMCGenerator(ChunkLoader<NodeChunk>* chunkLoader) :
		m_chunkLoader(chunkLoader)
	{

	}

	void NodesToMCGenerator::GenerateFromScratch(int x, int y)
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

		/*So now, if chunks had size CHUNK_SIZE, then mesh will require CHUNK_SIZE+1 nodes in x and z
		That means that exactly CHUNK_SIZE cubes is required!
		And the height must be CHUNK_HEIGHT - 1*/

		//Create cubes. In a heap. Becouse it's too big for a stack. Too bad, it will be slower. And yes, I guess we can forget about cash-optimization.
		std::vector<std::vector<std::vector<Cubie>>> AllTheCubies(CHUNK_SIZE, std::vector<std::vector<Cubie>>(CHUNK_HEIGHT - 1, std::vector<Cubie>(CHUNK_SIZE)));

		for (int ity = 0; ity < CHUNK_HEIGHT - 1; ity++)
		{
			
			//Fill the edge cubes.Ones that contains nodes from chunks other then centre
			for (int i = 0; i < CHUNK_SIZE - 1; i++)
			{
				//Fill the front-most
				AllTheCubies[i][ity][CHUNK_SIZE - 1].N0->N = &(chunkFront->NodeMap[i][ity][0]);
				AllTheCubies[i][ity][CHUNK_SIZE - 1].N4->N = &(chunkFront->NodeMap[i][ity + 1][0]);

				AllTheCubies[i][ity][CHUNK_SIZE - 1].N1->N = &(chunkFront->NodeMap[i + 1][ity][0]);
				AllTheCubies[i][ity][CHUNK_SIZE - 1].N5->N = &(chunkFront->NodeMap[i + 1][ity + 1][0]);

				AllTheCubies[i][ity][CHUNK_SIZE - 1].N3->N = &(chunkCentre->NodeMap[i][ity][CHUNK_SIZE - 1]);
				AllTheCubies[i][ity][CHUNK_SIZE - 1].N7->N = &(chunkCentre->NodeMap[i][ity + 1][CHUNK_SIZE - 1]);

				AllTheCubies[i][ity][CHUNK_SIZE - 1].N2->N = &(chunkCentre->NodeMap[i + 1][ity][CHUNK_SIZE - 1]);
				AllTheCubies[i][ity][CHUNK_SIZE - 1].N6->N = &(chunkCentre->NodeMap[i + 1][ity + 1][CHUNK_SIZE - 1]);
				AllTheCubies[i][ity][CHUNK_SIZE - 1].CalculateCubeval();

				//fill the right-most
				AllTheCubies[CHUNK_SIZE - 1][ity][i].N2->N = &(chunkRight->NodeMap[0][ity][i]);
				AllTheCubies[CHUNK_SIZE - 1][ity][i].N6->N = &(chunkRight->NodeMap[0][ity + 1][i]);

				AllTheCubies[CHUNK_SIZE - 1][ity][i].N1->N = &(chunkRight->NodeMap[0][ity][i + 1]);
				AllTheCubies[CHUNK_SIZE - 1][ity][i].N5->N = &(chunkRight->NodeMap[0][ity + 1][i + 1]);

				AllTheCubies[CHUNK_SIZE - 1][ity][i].N3->N = &(chunkCentre->NodeMap[CHUNK_SIZE - 1][ity][i]);
				AllTheCubies[CHUNK_SIZE - 1][ity][i].N7->N = &(chunkCentre->NodeMap[CHUNK_SIZE - 1][ity + 1][i]);

				AllTheCubies[CHUNK_SIZE - 1][ity][i].N0->N = &(chunkCentre->NodeMap[CHUNK_SIZE - 1][ity][i + 1]);
				AllTheCubies[CHUNK_SIZE - 1][ity][i].N4->N = &(chunkCentre->NodeMap[CHUNK_SIZE - 1][ity + 1][i + 1]);
				AllTheCubies[CHUNK_SIZE - 1][ity][i].CalculateCubeval();

			}
			//Fill the cube in the corner
			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].N1->N = &(chunkFrontRight->NodeMap[0][ity][0]);
			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].N5->N = &(chunkFrontRight->NodeMap[0][ity + 1][0]);

			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].N0->N = &(chunkFront->NodeMap[CHUNK_SIZE - 1][ity][0]);
			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].N4->N = &(chunkFront->NodeMap[CHUNK_SIZE - 1][ity + 1][0]);

			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].N3->N = &(chunkCentre->NodeMap[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1]);
			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].N7->N = &(chunkCentre->NodeMap[CHUNK_SIZE - 1][ity + 1][CHUNK_SIZE - 1]);

			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].N2->N = &(chunkRight->NodeMap[0][ity][CHUNK_SIZE - 1]);
			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].N6->N = &(chunkRight->NodeMap[0][ity + 1][CHUNK_SIZE - 1]);
			AllTheCubies[CHUNK_SIZE - 1][ity][CHUNK_SIZE - 1].CalculateCubeval();

			//And then the rest of cubes
			for (int itx = 0; itx < CHUNK_SIZE - 1; itx++)
			{
				for (int itz = 0; itz < CHUNK_SIZE - 1; itz++)
				{
					AllTheCubies[itx][ity][itz].N0->N = &(chunkCentre->NodeMap[itx][ity][itz + 1]);
					AllTheCubies[itx][ity][itz].N4->N = &(chunkCentre->NodeMap[itx][ity + 1][itz + 1]);

					AllTheCubies[itx][ity][itz].N1->N = &(chunkCentre->NodeMap[itx + 1][ity][itz + 1]);
					AllTheCubies[itx][ity][itz].N5->N = &(chunkCentre->NodeMap[itx + 1][ity + 1][itz + 1]);

					AllTheCubies[itx][ity][itz].N2->N = &(chunkCentre->NodeMap[itx + 1][ity][itz]);
					AllTheCubies[itx][ity][itz].N6->N = &(chunkCentre->NodeMap[itx + 1][ity + 1][itz]);

					AllTheCubies[itx][ity][itz].N3->N = &(chunkCentre->NodeMap[itx][ity][itz]);
					AllTheCubies[itx][ity][itz].N7->N = &(chunkCentre->NodeMap[itx][ity + 1][itz]);
					AllTheCubies[itx][ity][itz].CalculateCubeval();
				}
			}

		}
		
	}
	void NodesToMCGenerator::ProcessCube(std::vector<std::vector<std::vector<Cubie>>>& Cubies, int x, int y, int z)
	{
		Cubies[x][y][z].CalculateCubeval();
	}
	NodesToMCGenerator::MidPoint & NodesToMCGenerator::getMidPoint(std::vector<std::vector<std::vector<Cubie>>> &cubies, int cubeX, int cubeY, int cubeZ, int edgeID)
	{
		/*switch (edgeID)
		{
		case 0: {return cubies[cubeX][cubeY][cubeZ].N0->Right;}
		case 0: {return cubies[cubeX][cubeY][cubeZ].N0->Right;}
		}*/
	}
}
