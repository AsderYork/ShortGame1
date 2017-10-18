#pragma once
#include "NodeChunk.h"
#include <cassert>
#include <array>
#include "Ogre_Service.h"
#include "NodesToMCGenerator.h"
#include "MCToMesh.h"
#include <memory>

namespace GEM
{

	/**
	Controlls livetime of MarchingCubes Meshes.
	Intrenally it depends on ChunkLoader to pull neccessery information, on NodesToMCGenerator, to create Vertex and Index lists out of loaded maps, and on
	MCToMesh to Create actual meshes out of these lists
	*/
	class NodesToMCGeneratorController {
	public:

		NodesToMCGeneratorController(ChunkLoader<NodeChunk>* chunkLoader);

		/**!
		Generates chunk just from nodes
		This method performs chunk load from Loader by itself
		This method create actual mesh, for required chunk in a given position.
		This method should not be called for chunks that's allready exists
		*/
		void GenerateFromScratch(int x, int y, Ogre_Service* ogreService);

		/**!
		Update chunk that is allready created
		*/
		void UpdateChunk(int x, int y, Ogre_Service* ogreService);

	private:

		ChunkLoader<NodeChunk>* m_chunkLoader;

		std::vector<std::unique_ptr<NodesToMCGenerator>> m_generatedChunks;
		std::vector<std::unique_ptr<MCToMesh>> m_mcToMeshes;

	};


}