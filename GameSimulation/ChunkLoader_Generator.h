#pragma once
#include "LandscapeChunk.h"

namespace GEM::GameSim
{
	/**!
	Provides generation facility for chunks
	*/
	class ChunkLoader_Generator {
	public:
		
		/**!
		Creates chunk of cordinates (x,z) in a specified memoty position
		*/
		void FillChunkIn(int x, int z, LandscapeChunk* target);
	};
}