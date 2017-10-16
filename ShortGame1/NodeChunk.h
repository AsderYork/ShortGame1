#pragma once
#include "ChunkLoader.h"

#define CHUNK_SIZE 32
#define CHUNK_HEIGHT 64

namespace GEM
{
	struct Node {
		unsigned char Value=0;

		template <class Archive>
		void serialize(Archive & ar)
		{
			ar(Value);
		};
	};

	/**
	The chunks are arranged in interesting way.
	Every chunk have CHUNK_SIZE*CHUNK_SIZE nodes in a y-plane.
	And instead of
	________
	| * * * |
	| * * * |
	|_*_*_*_|

	Arrangement, where all nodes in the chunk are contained inside that chunk,
	so that edges of a chunk lies through cubes, formed by a nodes.
	Chunks will be arranged in a way, that left-most nodes will be right on the edge of a chunk
	the same for rear-most nodes. That way, only 3 additional chunks will be needed, to build a mesh! 

	

	*/
	struct NodeChunk : public ChunkBase
	{
		template <class Archive>
		void serialize(Archive & ar)
		{
			ar(NodeMap);
		};

		Node NodeMap[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];

		virtual void generateNewChunk() override;
	};
}