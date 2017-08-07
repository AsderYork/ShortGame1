#include "stdafx.h"
#include "ChunkLoader.h"
#include <cassert>

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>

#include <fstream>
#include <iostream>
#include <algorithm>


namespace GEM {


	ChunkLoader* ChunkLoader::m_singleton = nullptr;

	ChunkLoader::ChunkLoader()
	{
		assert(m_singleton == nullptr);//Otherwise singleton is allready set up
		m_singleton = this;
	}

	ChunkLoader::~ChunkLoader()
	{		
		SaveMagistral();
	}

	bool ChunkLoader::InitializeChunkLoader()
	{
		m_mapFolder = "../Map/";

		std::ifstream inputMagistral(m_mapFolder + "magistral", std::ios::binary);
		if (!inputMagistral.is_open())
		{
			LOGCATEGORY("ChunkLoader/InitializeChunkLoader").info("Map folder does not contain magistral");
		}
		else
		{
			cereal::BinaryInputArchive MagistralArchive(inputMagistral);
			MagistralArchive(m_magistral);
		}



		return false;
	}

	Chunk * ChunkLoader::getChunk(intpos2 pos)
	{
		//Check if chunk is allready loaded;
		auto loadedIter = m_loadedChunks.find(pos);
			//if it is, return it
			if (loadedIter != m_loadedChunks.end()) {return loadedIter->second;}

		//if it's not, check if it's in magistral
		auto magistralIter = std::find(m_magistral.begin(), m_magistral.end(), pos);
			//If it is, load it, add to a pool of loaded chunks, and then return. P.S. loadChunk do perform write in a pool
			if (magistralIter != m_magistral.end()) { return loadChunk(pos); }

		//if it's not in a magistral, create chunk, and add it to a pool
		return CreateChunk(pos);
	}

	void ChunkLoader::SaveMagistral()
	{
		std::ofstream outputMagistral(m_mapFolder + "magistral", std::ios::binary);
		cereal::BinaryOutputArchive MagistralArchive(outputMagistral);
		MagistralArchive(m_magistral);
	}

	Chunk * ChunkLoader::loadChunk(intpos2 pos)
	{
		//Check if it's in magistral. If it's not, it should be created instead;
		//This check will be in GetChunk;
		/*auto findResult = std::find(m_magistral.begin(), m_magistral.end(), pos);
		if (findResult == m_magistral.end())
		{
			LOGCATEGORY("ChunkLoader/loadChunk").info("Chunk %i:%i was atempted to be load, but it's not in a magistral, so it will be created instead");
		}*/

		//Perform actual loading
		std::ifstream inputChunk(m_mapFolder + "chunk" + std::to_string(IDfromXY(pos)), std::ios::binary);
		cereal::BinaryInputArchive ChunkArchive(inputChunk);
		Chunk* loadedChunk = new Chunk;

		ChunkArchive.loadBinary(loadedChunk, sizeof(Chunk));

		m_loadedChunks.insert(std::make_pair(loadedChunk->ChunkPos, loadedChunk));
		return loadedChunk;
	}

	void ChunkLoader::SaveChunk(Chunk* chunk)
	{
		
		//Check if it's allready in a magistral. If it's not, add it
		auto findResult = std::find(m_magistral.begin(), m_magistral.end(), chunk->ChunkPos);
		if (findResult == m_magistral.end())
		{//Then this chunk have never been saved before! Write it to a magistral
			m_magistral.push_back(chunk->ChunkPos);
		}

		std::ofstream outputChunk(m_mapFolder + "chunk" + std::to_string(chunk->id), std::ios::binary);		
		cereal::BinaryOutputArchive ChunkArchive(outputChunk);

		ChunkArchive.saveBinary(chunk, sizeof(Chunk));
	}

	Chunk * ChunkLoader::CreateChunk(intpos2)
	{
		Chunk* loadedChunk = new Chunk;
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT/2; y++)
			{
				for (int z = 0; z < CHUNK_SIZE; z++)
				{
					loadedChunk->Nodes[x][y][z].Value = 255;
				}
			}
		}
		m_loadedChunks.insert(std::make_pair(loadedChunk->ChunkPos, loadedChunk));
		return loadedChunk;
	}


	Chunk::~Chunk()
	{//Chunk should be saved before destruction
		ChunkLoader::m_singleton->SaveChunk(this);
		//And it should remove itself from a chunk pool
		auto iter = ChunkLoader::m_singleton->m_loadedChunks.find(this->ChunkPos);

#ifdef _DEBUG
		if (iter == ChunkLoader::m_singleton->m_loadedChunks.end())
		{
			LOGCATEGORY("Chunk/Destructor").crit("%i, %i chunk gets unloaded, but it's not in a Chunk Pool!", ChunkPos.x, ChunkPos.y);
			assert(false);
		}
#endif
		ChunkLoader::m_singleton->m_loadedChunks.erase(iter);
	}

}