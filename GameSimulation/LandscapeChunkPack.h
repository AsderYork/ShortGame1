#pragma once
#include "LandscapeChunk.h"
#include <vector>
#include <string>


namespace GEM::GameSim
{
	/**!
	Packs a bunch of landscapeChunks in a compressed manner and provide
	a decompression facility to get them back.
	*/
	class LandscapeChunkPack
	{
	private:
		std::string m_data;
		std::size_t m_initialSize;


	public:

		/**!
		Creates new pack and compresses it
		\param[in] Chunks A vector of pointers to chunks, that needed to be packed. Chunks must remain unchanged during that call
		*/
		LandscapeChunkPack(std::vector<const LandscapeChunk*> Chunks);

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(m_initialSize, m_data);
		}

		/**!
		Unpacks stored chunks.
		\returns Returns stored chunks in a vector.
		\note Decompression happens for every call, so it is recomended to call it once and store it result.
		*/
		std::vector<LandscapeChunk> UnpackLandscapeChunks();
	};
}