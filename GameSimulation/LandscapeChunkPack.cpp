#include "LandscapeChunkPack.h"
#include <zlib.h>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>

namespace GEM::GameSim
{
	LandscapeChunkPack::LandscapeChunkPack(std::vector<const LandscapeChunk*> Chunks)
	{
		std::stringstream PackedStream;
		{

			cereal::BinaryOutputArchive ar(PackedStream);

			ar(static_cast<uint32_t>(Chunks.size()));
			for (const auto& Chunk : Chunks)
			{
				ar((*Chunk));
			}
		}
		m_data = PackedStream.str();
		auto InitBufferSize = static_cast<uLongf>(m_data.size());
		std::string ResultBuff;
		//Reserve ammount is recomended in ZLib specs
		ResultBuff.resize(static_cast<std::size_t>(m_data.size()*1.1f + 12));
		auto OutroBuf = static_cast<uLongf>(m_data.size());
		auto result = compress(reinterpret_cast<Bytef*>(ResultBuff.data()), &OutroBuf, reinterpret_cast<Bytef*>(m_data.data()), InitBufferSize);
		
		m_data = ResultBuff.substr(0, OutroBuf);
		m_initialSize = InitBufferSize;
		

	};

	std::vector<LandscapeChunk> LandscapeChunkPack::UnpackLandscapeChunks()
	{
		std::string OutBuffer;
		OutBuffer.resize(m_initialSize);
		auto InitBufferSize = static_cast<uLongf>(m_initialSize);
		uncompress(reinterpret_cast<Bytef*>(OutBuffer.data()), &InitBufferSize, reinterpret_cast<Bytef*>(m_data.data()), static_cast<uLongf>(m_data.size()));
		m_data.swap(OutBuffer);

		std::stringstream Strbuf(m_data);

		cereal::BinaryInputArchive ar(Strbuf);
		uint32_t VecSize = 0;
		ar(VecSize);
		std::vector<LandscapeChunk> RetVec;
		RetVec.resize(VecSize);
		for (auto& Chunk : RetVec)
		{
			ar(Chunk);
		}
		return RetVec;
	}

};