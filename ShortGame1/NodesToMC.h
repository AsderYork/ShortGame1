#pragma once
#include "NodeChunk.h"
#include <cassert>
#include <array>
#include "Ogre_Service.h"


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

		/**
		Generates chunk just from nodes
		This method performs chunk load from Loader by itself
		This method create actual mesh, for required chunk in a given position.
		If required chunk allready created, it will be recreated, as cheap as possible
		*/
		void GenerateFromScratch(int x, int y, Ogre_Service* ogreService);
	private:

		/**!
		Fills MidPoints of a cube, based on a values of it's nodes
		\param[in] Cubies A Cubies, that contains nodes from which nodes come from
		\param[in] x X-coordinate of a cubie to be processed
		\param[in] y Y-coordinate of a cubie to be processed
		\param[in] z Z-coordinate of a cubie to be processed

		Cubies don't have to have cubevalue, it will be assigned inside.
		*/
		//void ProcessCube(Node*** NodeMap, int x, int y, int z);

		ChunkLoader<NodeChunk>* m_chunkLoader;

	};


}