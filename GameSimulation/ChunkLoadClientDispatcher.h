#pragma once
#include "LandscapeChunk.h"
#include "LandscapeChunkController.h"

#include "ChunkLoader_Generator.h"
#include "ChunkLoader_Storage.h"

#include "GameSimulation.h"
#include "ClientCommandDispatcher.h"
#include "LandscapeSystem_ClientProcessor.h"
#include "LandscapeMeshGenerator.h"

#include <list>
#include <memory>



namespace GEM::GameSim
{
	/**!
	Manages chunks on the client. Loading from network and disk. Requesting neccessary chunks
	and so on.
	*/
	class ChunkLoadClientDispatcher
	{
	public:
		/**!
		Chunks on the client might be outdated if they restored from disk. So for every chunk in the storage
		we also hold flag if they're confirmed by the server.
		*/
		struct StoredChunk
		{
			LandscapeChunk chunk;
			bool isConfirmed;
			std::unique_ptr<LandscapeMeshGenerator> mesh;
		};
	private:
		LandscapeChunkController m_chunkController;
		LandscapeSystemClientProcessor m_landscapeProcessor;

		ChunkLoader_Storage m_chunkLoader;


		/**!
		Creates new place in a storage for this chunk
		If chunk with a given cordinates allready present, a pointer to it will be returned
		without affecting this chunk in any way or creating new chunks.
		*/
		StoredChunk* addNewChunk(int x, int z);

		friend class LandscapeSystemClientProcessor;
		void FormRequest(ClientCommandDispatcher* MessageDispatcher);

		void DoMeshes();

	public:
		std::list<StoredChunk> m_chunks;

		ChunkLoadClientDispatcher() : m_landscapeProcessor(this) {}

		/**!
		Returns a pointer to a chunk in a storage.
		\returns Returs 0 if chunk is not present, otherwise returns a pointer to a chunk with given position
		*/
		StoredChunk* getChunk(int x, int z);

		inline LandscapeSystemClientProcessor& getProcessor() { return m_landscapeProcessor; }
		inline LandscapeChunkController& getController() { return m_chunkController; }

		void Process(ClientCommandDispatcher* MessageDispatcher);


	};
}