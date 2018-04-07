#include "ChunkLoader_Generator.h"

namespace GEM::GameSim
{

	void ChunkLoader_Generator::FillChunkIn(int x, int z, LandscapeChunk * target)
	{
		target->setVersion(1);
		target->setPosX(x);
		target->setPosZ(z);



		for (int y = 0; y < LandscapeChunk_Height; y++)
		{
			uint8_t val = y < 20 ? 255 : 0;
			for (int z = 0; z < LandscapeChunk_Size; z++)
			{
				for (int x = 0; x < LandscapeChunk_Size; x++)
				{
					target->getNode(x, y, z).Value = val;
				}
			}
		}
	}

}