#pragma once

#include "ChunkLoader.h"
#include "MarchingCubes.h"
#include "MarchingToOgre.h"

#include <vector>


//Every chunk is Cuboid. The lenght and width is the same, height is different and bigger

namespace GEM
{


	class VoxelMap {
	public:
		VoxelMap();
		/**
		Every time player moves from chunk to chunk, new chunks must be loaded and old one should be unloaded.
		Even more, the chunkMap starts with (0,0) in bottom left corner and ends with (MAP_SIZE,MAP_SIZE) in a top right,
		but the actual game map have a center in a (0,0) and goes from (-MAP_SIZE/2, - MAP_SIZE/2) to (MAP_SIZE/2, MAP_SIZE/2)
		So this methods fixes this differences and translates Player position in a map, to a Chunk coordinates, on which he standing on
		*/
		void SetPlayerPos(int x, int y);

		inline intpos2 fromWorldPosToChunkPos(int x, int y)
		{
			
			x = x / CHUNK_SIZE;
			y = y / CHUNK_SIZE;
			x += MAP_SIZE / 2;
			y += MAP_SIZE / 2;

			if ( !((x < MAP_SIZE) && (x >= 0) && (y < MAP_SIZE) && (y >= 0)))
			{
				LOGCATEGORY("VoxelMap/fromWorldPosToChunkPos").crit("PlayerChunk [%i:%i] is out of bounds of map!", x, y);
			}
				return intpos2(x, y);
		};

		void ResizeChunkMap(int NewRadius);

		MarchingCubiesMeshCalc* getMeshCalc();
		~VoxelMap();
	private:

		void UpdateChunkListForCurrentPosition(intpos2 CurrentPosition);


		ChunkLoader* m_chunkLoader;
		MarchingCubiesMeshCalc m_cubesCalculator;
		//MarchingToOgre m_marchingToOgre;

		int m_loadRadius = 4;//How many chunks must be loaded around one, that is containing player now;
		intpos2 m_playerChunk;//Position of a chunk, currently occupied by a player;

		//std::vector<std::vector<Chunk*>> m_chunkMap;
		std::vector<std::shared_ptr<Chunk>> m_chunkMap;

	};
}