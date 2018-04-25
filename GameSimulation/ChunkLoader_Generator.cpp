#include "ChunkLoader_Generator.h"

namespace GEM::GameSim
{

	void ChunkLoader_Generator::FillChunkIn(int x, int z, LandscapeChunk * target)
	{
		target->setVersion(1);
		target->setPosX(x);
		target->setPosZ(z);

		std::vector<std::vector<float>> Heights;

		float PosX, PosZ;
		for (int tz = 0; tz < LandscapeChunk_Size; tz++)
		{
			Heights.emplace_back();
			for (int tx = 0; tx < LandscapeChunk_Size; tx++)
			{
				PosX = x + tx;
				PosZ = z + tz;

				Heights[tz].push_back(20.0f + sinf(PosX / 100) * 12 + sinf(PosZ / 143) * 8 + cosf(PosX / 10 + PosZ / 4) * 4);
			}
		}

		for (int y = 0; y < LandscapeChunk_Height; y++)
		{
			
			for (int z = 0; z < LandscapeChunk_Size; z++)
			{
				for (int x = 0; x < LandscapeChunk_Size; x++)
				{
					uint8_t val;
					if (Heights[x][z] < y)
					{
						val = 255;
					}
					else if (ceilf(Heights[x][z]) < y)
					{
						val = 0;
						//val = (Heights[x][z] - y) * 255;
					}
					else { val = 0; }
					target->getNode(x, y, z).Value = val;
				}
			}
		}
	}

}