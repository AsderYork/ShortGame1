#pragma once
#include "LandscapeChunk.h"
#include "LandscapeChunkController.h"

#include "ChunkLoader_Generator.h"
#include "ChunkLoader_Storage.h"

#include "GameSimulation.h"
#include "ClientCommandDispatcher.h"

#include <list>



namespace GEM::GameSim
{

	class ChunkLoadClientDispatcher
	{
	private:
		LandscapeChunkController & m_chunkController;

		ChunkLoader_Storage m_chunkLoader;

		/**!
		Chunks on the client might be outdated if they restored from disk. So for every chunk in the storage
		we also hold flag if they're confirmed by the server.
		*/
		struct StoredChunk
		{
			LandscapeChunk chunk;
			bool isConfirmed;
		};

		std::list<LandscapeChunk> m_chunks;
	public:

		ChunkLoadClientDispatcher(LandscapeChunkController& ChunkController) : m_chunkController(ChunkController) {}

		void FormRequest(ClientCommandDispatcher* MessageDispatcher);

	};
}