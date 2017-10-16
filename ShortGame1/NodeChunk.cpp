#include "stdafx.h"
#include "NodeChunk.h"



namespace GEM
{
	void GEM::NodeChunk::generateNewChunk()
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT/2; y++)
			{
				for (int z = 0; z < CHUNK_SIZE; z++)
				{
					NodeMap[x][y][z].Value = 255;
				}
			}

			for (int y = CHUNK_HEIGHT /2; y <  CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < CHUNK_SIZE; z++)
				{
					NodeMap[x][y][z].Value = 0;
				}
			}

		}

		NodeMap[7][40][7].Value = 255;
		NodeMap[7][40][8].Value = 255;
		NodeMap[7][40][9].Value = 255;


		NodeMap[9][40][7].Value = 255;
		NodeMap[9][40][8].Value = 255;
		NodeMap[9][40][9].Value = 255;
	}
}
