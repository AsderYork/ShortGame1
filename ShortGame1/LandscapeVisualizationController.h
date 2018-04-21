#pragma once
#include "LandscapeVisualization.h"
#include "Ogre_Service.h"
#include "GS_Client.h"
#include <map>
#include <LandscapeChunkStorageListener.h>

namespace GEM
{
	class LandscapeVisualizationController : GameSim::ChunkStorageListener<LandscapeVisualMesh>
	{
		GS_Client* m_client;
		
	public:
		inline LandscapeVisualizationController(GS_Client* client, Ogre_Service* ogre) : m_client(client)
		{
			client->m_chunkDispatcher.m_chunks.RegisterListener(this);
		}

		std::vector<LandscapeVisualMesh> viss;

		virtual void NewChunkAdded(GameSim::LandscapeChunk * NewChunk, GameSim::LandscapeMesh* newMesh, LandscapeVisualMesh* VisMesh) override;
		virtual void ChunkRemoved(GameSim::LandscapeChunk * NewChunk, GameSim::LandscapeMesh* newMesh, LandscapeVisualMesh* VisMesh) override;
	};
}