#pragma once
#include "stdafx.h"
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>

namespace GEM
{
	/**
	Loads chunks. Uses Cereal.
	Chunks of any type can be loaded!
	ChunkType MUST be cereal-serializeble!
	If chunk is not loaded, it must be created somehow! I don't know how
	*/
	template <class ChunkType>
	class ChunkLoader
	{
	public:
		using CordShared = std::pair<std::pair<int, int>, std::shared_ptr<ChunkType>>;
		using CordWeak = std::pair<std::pair<int, int>, std::weak_ptr<ChunkType>>;

		ChunkLoader(std::string chunkPath, std::string ChunkPostfix);

		/**
		Returns a shared_ptr to a chunk located at x,y. While at least someone holds a shared_ptr to this chunk, it will remain loaded and
		evry call to this method will return shared_ptr to exactly that chunk. But if all shared_ptr's to a particualar chunk is destroyed,
		chunk will be unloaded(with save) and call to a GetChunk again for this chunk will be much slower.
		*/
		std::shared_ptr<ChunkType> GetChunk(int x, int y);
		/**
		Forces chunk to be saved
		*/
		//void SaveChunk(std::shared_ptr<ChunkType> Chunk);
	private:

		//List of all chunks were created and actually exist.
		std::vector<std::pair<int, int>> m_magistral;
		//List of all chunks, that are actually loaded.
		std::vector<CordWeak> m_loadedChunks;

		//Prefix to chunk names
		std::string m_chunksPath;
		//Postfix to chunk names
		std::string m_chunksPostfix;

		//Loads magistral. Must be called before chunk-loading!
		void LoadMagistral();
		//Save magistral. Shuld be called, so that newely created chunk would actually exist.
		void SaveMagistral();

		//Loads chunk from disk. if chunk exists, otherwise nullptr
		CordShared LoadChunk(int x, int y)
		{
			return CordShared();
		}

		//Creates chunk from scratch. if chunk exists, otherwise nullptr
		//CordShared CreateChunk(int x, int y);

	};

	

	template<class ChunkType>
	inline ChunkLoader<ChunkType>::ChunkLoader(std::string chunkPath, std::string ChunkPostfix) :
		m_chunksPath(chunkPath),
		m_chunksPostfix(ChunkPostfix)
	{
		LoadMagistral();
		m_magistral.push_back(std::make_pair(12, 44));
		SaveMagistral();
	}

	template<class ChunkType>
	inline std::shared_ptr<ChunkType> ChunkLoader<ChunkType>::GetChunk(int x, int y)
	{
		for (auto &aChunk : m_loadedChunks)
		{
			if ((aChunk.first.first == x) && (aChunk.first.first == y))
			{
				assert(!aChunk.second.expired());//Chunk can't be expired in this list!


			}
		}
		return std::shared_ptr<ChunkType>();
	}

	template<class ChunkType>
	inline void ChunkLoader<ChunkType>::LoadMagistral()
	{
		std::ifstream inputMagistral(m_chunksPath + "magistral" + m_chunksPostfix, std::ios::binary);
		if (!inputMagistral.is_open())
		{
			LOGCATEGORY("ChunkLoader/LoadMagistral").warn("Magistral file is not exist. It will be created on save. <prefix:%s; postfix:%s;>", m_chunksPath.c_str(), m_chunksPostfix.c_str());
			return;
		}
		cereal::BinaryInputArchive MagistralArchive(inputMagistral);
		MagistralArchive(m_magistral);
	}

	template<class ChunkType>
	inline void ChunkLoader<ChunkType>::SaveMagistral()
	{
		std::ofstream outputMagistral(m_chunksPath + "magistral" + m_chunksPostfix, std::ios::binary);
		cereal::BinaryOutputArchive MagistralArchive(outputMagistral);
		MagistralArchive(m_magistral);
	}

}