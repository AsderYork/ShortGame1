#include "ChunkLoader_Generator.h"

namespace GEM::GameSim
{

	void ChunkLoader_Generator::FillChunkIn(int x, int z, LandscapeChunk * target)
	{
		target->setVersion(1);
		target->setPosX(x);
		target->setPosZ(z);

		auto[ActualX, ActualZ] = target->getPosition();

		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				int ThisX = (ActualZ * 16) + x;
				int ThisZ = (ActualX * 16) + z;
				float NodeHeight = 20 + sin(ThisX / 3.0f) * 2 + sin(ThisZ / 3.83f + 1)*1.43f;

				for (int y = 0; y < (int)floor(NodeHeight); y++)
				{
					target->getNode(z, y, x).Value = 255;
				}
				target->getNode(z, (int)floor(NodeHeight), x).Value = (int)(255 * (NodeHeight - floor(NodeHeight)));

			}
		}

	}

}