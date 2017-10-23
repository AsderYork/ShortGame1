#pragma once
#include "NodeChunk.h"
#include <cassert>
#include <array>
#include "Ogre_Service.h"
#include "NodesToMCGenerator.h"
#include "NTMCG_Naive.h"
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
		Unloads chunk, removing generator and MCtoMesh associated with it.
		If specified chunk wasn't loaded, nothing will happen.
		*/
		void UnloadChunk(int x, int z);

		/**!
		Sometimes you just need to unload everything. This method does axactly that
		*/
		void UnloadAllChunks();

		/**!
		Update chunk that is allready created
		*/
		void UpdateChunk(int x, int y, Ogre_Service* ogreService);

		/**
		Mark node as changed in corresponding generator, if there is one. Or just do nothing.
		This method dont perform actual node alteration! It's just tells generators, that cubes around node must be updated
		*/
		void ChangeNode(int ChunkX, int ChunkZ, int X, int Y, int Z);

	private:

		ChunkLoader<NodeChunk>* m_chunkLoader;

		std::vector<std::unique_ptr<NTMCG_Base>> m_chunkGenerators;
		std::vector<std::unique_ptr<MCToMesh>> m_mcToMeshes;

	};


}