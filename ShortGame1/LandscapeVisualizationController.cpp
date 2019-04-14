#include "stdafx.h"
#include "LandscapeVisualizationController.h"

namespace GEM
{	
	void LandscapeVisualizationController::NewChunkAdded(GameSim::LandscapeChunk * NewChunk, GameSim::LandscapeMesh* newMesh, LandscapeVisualMesh* VisMesh)
	{
		*VisMesh = LandscapeVisualization::GenerateVisualMesh(newMesh, NewChunk);
	}
	void LandscapeVisualizationController::ChunkRemoved(GameSim::LandscapeChunk * NewChunk, GameSim::LandscapeMesh* newMesh, LandscapeVisualMesh* VisMesh)
	{
		VisMesh->Clear();
	}
}