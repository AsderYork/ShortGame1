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

		/**!
		Creates empty chunkPack
		*/
		LandscapeChunkPack() {};

		void operator=(LandscapeChunkPack &&other) {
			m_data = std::move(other.m_data);
			m_initialSize = other.m_initialSize;

			other.m_initialSize = 0; 
		}


		template<class Archive>
		void save(Archive & archive) const
		{
			archive(m_initialSize);

			archive(cereal::make_size_tag(static_cast<uint32_t>(m_data.size())));
			archive(cereal::binary_data(m_data.data(), m_data.size()));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(m_initialSize);

			uint32_t strSize;
			archive(cereal::make_size_tag(strSize));

			m_data.resize(static_cast<std::size_t>(strSize));
			archive(cereal::binary_data(m_data.data(), static_cast<std::size_t>(strSize)));

		}


		/**!
		Unpacks stored chunks.
		\returns Returns stored chunks in a vector.
		\note Decompression happens for every call, so it is recomended to call it once and store it result.
		*/
		std::vector<LandscapeChunk> UnpackLandscapeChunks() const;
	};
}