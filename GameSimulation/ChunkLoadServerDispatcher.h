#pragma once
#include "LandscapeChunk.h"
#include "LandscapeChunkController.h"

#include "ChunkLoader_Generator.h"
#include "ChunkLoader_Storage.h"

#include "LandscapeSystem_ServerProcessor.h"

#include <list>



namespace GEM::GameSim
{
	
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

		std::list<LandscapeChunk> m_chunks;

		/**!
		Sever must save the state of the map from time to time.
		This method decides if it's time to save chunks and does so, when it' sees fit
		*/
		void PerformOccasionalSave();

		/**!
		Creates new place in a storage for this chunk
		If chunk with a given cordinates allready present, a pointer to it will be returned
		without affecting this chunk in any way or creating new chunks.
		*/
		LandscapeChunk* addNewChunk(int x, int z);

	public:

		/**!
		Returns a pointer to a chunk in a storage.
		\returns Returs 0 if chunk is not present, otherwise returns a pointer to a chunk with given position
		*/
		LandscapeChunk* getChunk(int x, int z);

		ChunkLoadServerDispatcher(PlayerController& PlayerController) :
			m_processor(PlayerController, this) {}
		
		inline LandscapeSystemServerProcessor& getProcessor() { return m_processor; }

		void ProcessChunks();

		void Start();

		inline LandscapeChunkController& getChunkController() { return m_chunkController; }

	};
}