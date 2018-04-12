#pragma once
#include "LandscapeChunk.h"
#include "LandscapeChunkController.h"

#include "ChunkLoader_Generator.h"
#include "ChunkLoader_Storage.h"

#include <list>



namespace GEM::GameSim
{
	
	class ChunkLoadServerDispatcher
	{
	private:
		LandscapeChunkController m_chunkController;

		ChunkLoader_Generator m_chunkGenerator;
		ChunkLoader_Storage m_chunkLoader;

		std::list<LandscapeChunk> m_chunks;

		/**!
		Sever must save the state of the map from time to time.
		This method decides if it's time to save chunks and does so, when it' sees fit
		*/
		void PerformOccasionalSave();

	public:
		void ProcessChunks();

		void Start();

		inline LandscapeChunkController& getChunkController() { return m_chunkController; }

	};
}