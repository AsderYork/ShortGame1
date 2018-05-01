#pragma once
#include <LandscapeChunk.h>
#include <LandscapeChunkController.h>

#include <ChunkLoader_Generator.h>
#include <ChunkLoader_Storage.h>

#include <GameSimulation.h>
#include <ClientCommandDispatcher.h>
#include "LandscapeSystem_ClientProcessor.h"
#include <LandscapeMeshGenerator.h>
#include <LandscapeChunkStorage.h>

#include "LandscapeVisualization.h"
#include "LandscapePhysics.h"

#include <memory>



namespace GEM
{
	struct ChunkConfirmationEnsurance
	{
		bool isConfirmed;
	};

	using ChunkStorageType = GameSim::ChunkStorage<ChunkConfirmationEnsurance, LandscapeVisualMesh, GameSim::PerChunkCollisionObject>;
	/**!
	Manages chunks on the client. Loading from network and disk. Requesting neccessary chunks
	and so on.
	*/
	class ChunkLoadClientDispatcher
	{
	private:
		GameSim::LandscapeChunkController m_chunkController;
		LandscapeSystemClientProcessor m_landscapeProcessor;

		GameSim::ChunkLoader_Storage m_chunkLoader;


		friend class LandscapeSystemClientProcessor;
		void FormRequest(GameSim::ClientCommandDispatcher* MessageDispatcher);



	public:
		ChunkStorageType m_chunks;

		ChunkLoadClientDispatcher() : m_landscapeProcessor(this) {}


		inline LandscapeSystemClientProcessor& getProcessor() { return m_landscapeProcessor; }
		inline GameSim::LandscapeChunkController& getController() { return m_chunkController; }

		void Process(GameSim::ClientCommandDispatcher* MessageDispatcher);


	};
}