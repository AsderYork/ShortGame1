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

		NodeMap[0][33][0].Value = 255;

		NodeMap[7][40][7].Value = 255;
		NodeMap[7][40][8].Value = 255;
		NodeMap[7][40][9].Value = 255;


		NodeMap[9][40][7].Value = 255;
		NodeMap[9][40][8].Value = 255;
		NodeMap[9][40][9].Value = 255;
		
		//A Cube
		NodeMap[20][40][20].Value = 255;
		NodeMap[20][40][21].Value = 255;
		NodeMap[20][40][22].Value = 255;

		NodeMap[21][40][20].Value = 255;
		NodeMap[21][40][21].Value = 255;
		NodeMap[21][40][22].Value = 255;

		NodeMap[22][40][20].Value = 255;
		NodeMap[22][40][21].Value = 255;
		NodeMap[22][40][22].Value = 255;

		NodeMap[20][41][20].Value = 255;
		NodeMap[20][41][21].Value = 255;
		NodeMap[20][41][22].Value = 255;

		NodeMap[21][41][20].Value = 255;
		NodeMap[21][41][21].Value = 255;
		NodeMap[21][41][22].Value = 255;

		NodeMap[22][41][20].Value = 255;
		NodeMap[22][41][21].Value = 255;
		NodeMap[22][41][22].Value = 255;

		NodeMap[20][42][20].Value = 255;
		NodeMap[20][42][21].Value = 255;
		NodeMap[20][42][22].Value = 255;

		NodeMap[21][42][20].Value = 255;
		NodeMap[21][42][21].Value = 255;
		NodeMap[21][42][22].Value = 255;

		NodeMap[22][42][20].Value = 255;
		NodeMap[22][42][21].Value = 255;
		NodeMap[22][42][22].Value = 255;
	}
}
