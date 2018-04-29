#include "stdafx.h"
#include "LandscapeVisualizationController.h"

namespace GEM
{

	void LandscapeVisualizationController::NewChunkAdded(GameSim::LandscapeChunk * NewChunk, GameSim::LandscapeMesh* newMesh, LandscapeVisualMesh* VisMesh)
	{
		//Test
		auto[ActualX, ActualZ] = NewChunk->getPosition();

		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				int ThisX = (ActualZ * 16) + x;
				int ThisZ = (ActualX * 16) + z;
				float NodeHeight = 20 + sin(ThisX / 3.0f) * 2 + sin(ThisZ / 3.83f + 1)*1.43f;

				for (int y = 0; y < (int)floor(NodeHeight); y++)
				{
					if (NewChunk->getNode(z, y, x).Value != 255) {
						printf("Chunk[%i;%i] differs in position (%i;%i;%i); Have %i but should %i!\n", ActualX, ActualZ, x,y,z, NewChunk->getNode(z, y, x).Value, 255);
					}
				}

				if (NewChunk->getNode(z, (int)floor(NodeHeight), x).Value != (int)(255 * (NodeHeight - floor(NodeHeight)))) {
					printf("Chunk[%i;%i] differs in position (%i;%i;%i); Have %i but should %i!\n", ActualX, ActualZ, x, (int)floor(NodeHeight), z, NewChunk->getNode(z, (int)floor(NodeHeight), x).Value, (int)(255 * (NodeHeight - floor(NodeHeight))));
				}
			}
		}


		*VisMesh = LandscapeVisualization::GenerateVisualMesh(newMesh, NewChunk->getPosition());
	}
	void LandscapeVisualizationController::ChunkRemoved(GameSim::LandscapeChunk * NewChunk, GameSim::LandscapeMesh* newMesh, LandscapeVisualMesh* VisMesh)
	{
		VisMesh->Clear();
	}
}