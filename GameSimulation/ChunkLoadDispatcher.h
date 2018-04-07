#pragma once
#include "LandscapeChunk.h"
#include "LandscapeChunkController.h"

#include "ChunkLoader_Generator.h"
#include "ChunkLoader_Storage.h"

#include <list>



namespace GEM::GameSim
{
	
	class ChunkLoadDispatcher_Server
	{
	private:
		LandscapeChunkController & m_chunkController;

		ChunkLoader_Generator m_chunkGenerator;
		ChunkLoader_Storage m_chunkLoader;

		std::list<LandscapeChunk> m_chunks;
	public:

		void ProcessChunksRequirements();

	};
}