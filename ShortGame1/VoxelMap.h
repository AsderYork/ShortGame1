#pragma once
#include <vector>

//Every chunk is Cuboid. The lenght and width is the same, height is different and bigger

namespace GEM
{


	class VoxelMap {
	public:


	private:

		int m_loadRadius = 4;//How many chunks must be loaded around one, that is containing player now;
		intpos2 PlayerChunk;//Position of a chunk, currently occupied by a player;

		std::vector<std::vector<Chunk*>> m_chunkMap;


	};
}