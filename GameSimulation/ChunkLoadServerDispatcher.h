#pragma once
#include "LandscapeChunk.h"
#include "LandscapeChunkController.h"

#include "ChunkLoader_Generator.h"
#include "ChunkLoader_Storage.h"

#include "LandscapeSystem_ServerProcessor.h"
#include "LandscapeChunkStorage.h"

#include "LandscapePhysics.h"



namespace GEM::GameSim
{
	using ChunkStorageType = ChunkStorage<PerChunkCollisionObject>;

	/**!
	Completely manages server-side chunk's stuff. Creates chunks when needed, stores them
	on disk and in memory, responds to client's requests and so on.
	*/
	class ChunkLoadServerDispatcher
	{
	private:
		LandscapeChunkController m_chunkController;

		ChunkLoader_Generator m_chunkGenerator;
		ChunkLoader_Storage m_chunkLoader;

		LandscapeSystemServerProcessor m_processor;

		ChunkStorageType m_chunks;

		/**!
		Sever must save the state of the map from time to time.
		This method decides if it's time to save chunks and does so, when it' sees fit
		*/
		void PerformOccasionalSave();

		/**!
		Performs loading or creation of one chunk
		*/
		void getOneChunk(int x, int z);

	public:

		ChunkLoadServerDispatcher(PlayerController& PlayerController) :
			m_processor(PlayerController, this) {}
		
		inline LandscapeSystemServerProcessor& getProcessor() { return m_processor; }

		/**!
		Forces chunk (x,z) to be completely loaded,
		meaning that it will have mesh and every per-chunk data prepared.
		*/
		void ForceChunkCreation(int x, int z);

		void ProcessChunks();

		void Start();

		inline LandscapeChunkController& getChunkController() { return m_chunkController; }

		inline ChunkStorageType& getStorage() { return m_chunks; }

	};
}