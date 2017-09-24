#include "stdafx.h"
#include "NodeChunk.h"



namespace GEM
{
	void GEM::NodeChunk::generateNewChunk()
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < CHUNK_SIZE; z++)
				{
					NodeMap[x][y][z].Value = 255;
				}
			}
		}
	}
}
