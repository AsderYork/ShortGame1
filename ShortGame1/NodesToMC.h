#pragma once
#include "NodeChunk.h"

namespace GEM
{
	/**
	Probably should generate MarchingCubeie-mesh out of a chunk
	*/
	class NodesToMCGenerator {
	public:

		struct Cubie {
			/**
			ID's from http://paulbourke.net/geometry/polygonise/
			And all the scheme, actually; And (0,0,0) node is left front bottom
			*/
			Node* N0 = nullptr;
			Node* N1 = nullptr;
			Node* N2 = nullptr;
			Node* N3 = nullptr;

			Node* N4 = nullptr;
			Node* N5 = nullptr;
			Node* N6 = nullptr;
			Node* N7 = nullptr;

			unsigned short cubeval = 0;
		};
		NodesToMCGenerator(ChunkLoader<NodeChunk>* chunkLoader);

		/**
		Generates chunk just from nodes
		*/
		void GenerateFromScratch(int x, int y);
	private:
		ChunkLoader<NodeChunk>* m_chunkLoader;

	};
}