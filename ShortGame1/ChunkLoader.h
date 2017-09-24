#pragma once
#include "stdafx.h"


#include <type_traits>

#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <functional>
#include <algorithm>

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>

namespace GEM
{
	template<class ChunkType>
	class ChunkLoader;

	struct intcord2
	{
		int x;
		int y;
		intcord2(): x(0), y(0) {}
		intcord2(int _x, int _y) : x(_x), y(_y) {}
		template <class Archive>
		void serialize(Archive & ar)
		{
			ar(x, y);
		}
		bool operator==(const intcord2& other) const {
			if ((other.x == x) && (other.y == y)) { return true; }
			return false;
		}

	};

	/**
	\brief Base for all the chunks, that should be controlled by a ChunkLoader

	If something is want to use ChunkLoader system, it must derive from this class.
	Constructors allways must be trivial! This system can't pass parameters.
	*/
	class ChunkBase {
	public:

		template <class Archive>
		void serialize(Archive & ar)
		{
			ar(cords);
		};

		virtual ~ChunkBase() {
			m_deleteCallback();
		}

	protected:
		/**
		\brief Creates a new chunk, if the requested chunk is actualy not exist.
		\note If creating a chunk do request some outer info, a singleton pattern might be used, to pass it to instance beeing created.
		*/
		virtual void generateNewChunk()=0;
		intcord2 getCords() const
		{
			return cords;
		};
		
	private:
		template<class ChunkType>
		friend class ChunkLoader;
		//Cordinates of a chunk.
		intcord2 cords;

		/**
		Get's called in the destructor and removes object from ChunkLoader's pool
		*/
		std::function<void()> m_deleteCallback;

		/** Pointer to a ChunkLoader that owns this chunk. Used for removing chunk from chunklist when it get's destroyed*/

		//template<class ChunkType>
		//ChunkLoader* m_owner;
	};

	/**
	Loads chunks. It can hash chunk pointers, so that chunk will stay loaded, while someone is using it. Prevents from nultiple copies
	of a chunk in memory. It also make aquireing of chunk opaque to a user, so that it'll make no difference to them, where chunk existed, before
	they asked for them, or it were created just now(except speed of course, I'm not a magician after all)
	*/
	template<class ChunkType>
	class ChunkLoader
	{
		static_assert(std::is_base_of<ChunkBase, ChunkType>::value);/**ChunkLoader can use only childrens of a ChunkBase as a template Parameter! */
	public:
		ChunkLoader(std::string preftx, std::string postfix) : m_prefix(preftx), m_postfix(postfix) { loadMagistral(); }

		/**
		\brief Returns a chunk with said position. If this chunk was never existed before, it will be created, if it's allready exists, it will be
		loaded and returned. If it's allready loaded, it will be just returned, without load of another instance! So be carefult, it's shared!
		*/
		std::shared_ptr<ChunkType> getChunk(int x, int y);

		/**
		Saves a chunk. Chunks also get saved when they removed from memory, but if you, for some reason,
		thinks it must be saved RIGHT NOW!!!!!11!! it's possible
		*/
		void saveChunk(ChunkType* Chunk);

		/**
		Magistral contains reference to all chunks, that were created and saved. This method get's called at the destructor of this class
		But if you, for some reason, want to save all the changes to a magistral early, you can do that!
		*/
		void saveMagistral();

	private:

		//std::vector<std::weak_ptr<ChunkType>> m_chunkPool;
		std::vector<std::pair<intcord2, std::weak_ptr<ChunkType>>> m_chunkPool;
		std::vector<intcord2> m_magistral;

		/**
		There are a lot of chunkLoaders now. And where they should store their files?
		To answer that question, every chunkLoader now uses prefixes and postfixes to every files they use. The actuall files they
		create is ordered in a flat structure.
		*/
		std::string m_prefix;
		std::string m_postfix;


		void loadMagistral();


		std::shared_ptr<ChunkType> createChunk(intcord2 pos);
		std::shared_ptr<ChunkType> loadChunk(intcord2 pos);

	};
	
	template<class ChunkType>
	inline std::shared_ptr<ChunkType> ChunkLoader<ChunkType>::getChunk(int x, int y)
	{
		//Check if it's loaded allready
		for (auto& chunk : m_chunkPool)
		{
			if (chunk.second.lock().get()->cords == intcord2(x, y))
			{
				//assert(!chunk.second.expired());//Chunk can't be expired!
				return chunk.second.lock();
			}
		}

		//If it's not loaded, check if it's in magistral
		for (auto& magistralItem : m_magistral)
		{
			if (magistralItem == intcord2(x, y))
			{
				return loadChunk(intcord2(x, y));
			}
		}

		//Otherwise just create it!
		return createChunk(intcord2(x,y));
	}

	template<class ChunkType>
	inline void ChunkLoader<ChunkType>::saveChunk(ChunkType* Chunk)
	{
		auto pos = Chunk->cords;
		std::ofstream inputChunk(m_prefix + "chunk" + std::to_string(pos.x) + "a" + std::to_string(pos.y) + m_postfix, std::ios::binary);
		cereal::BinaryOutputArchive ChunkArchive(inputChunk);

		if (std::find(m_magistral.begin(), m_magistral.end(), Chunk->cords) == m_magistral.end())
		{
			m_magistral.push_back(Chunk->cords);
		}

		ChunkArchive(Chunk->cords);
		ChunkArchive(*Chunk);
	}

	template<class ChunkType>
	inline void ChunkLoader<ChunkType>::saveMagistral()
	{
		std::ofstream outputMagistral(m_prefix + "magistral" + m_postfix, std::ios::binary);
		cereal::BinaryOutputArchive MagistralArchive(outputMagistral);
		MagistralArchive(m_magistral);
	}

	template<class ChunkType>
	inline void ChunkLoader<ChunkType>::loadMagistral()
	{
		std::ifstream inputMagistral(m_prefix + "magistral" + m_postfix, std::ios::binary);

		if (!inputMagistral.is_open()) {
			LOGCATEGORY("ChunkLoader/loadMagistral").warn("Magistral can't be loaded! %s, %s", m_prefix.c_str(), m_postfix.c_str());
			return;
		}
		cereal::BinaryInputArchive MagistralArchive(inputMagistral);
		MagistralArchive(m_magistral);
	}

	template<class ChunkType>
	inline std::shared_ptr<ChunkType> ChunkLoader<ChunkType>::createChunk(intcord2 pos)
	{
		auto ptr = std::make_shared<ChunkType>();
		static_cast<ChunkBase*>(ptr.get())->cords = pos;
		static_cast<ChunkBase*>(ptr.get())->generateNewChunk();
		static_cast<ChunkBase*>(ptr.get())->m_deleteCallback = [this, chunkThis = ptr.get()]() {

			for (auto it = m_chunkPool.begin(); it != m_chunkPool.end(); it++)
			{
				if (it->first == chunkThis->cords) {
					m_chunkPool.erase(it);
					saveChunk(chunkThis);
					return;
				}
			}
			LOGCATEGORY("LAMBDA").error("Cant find deletee in a pool!");
		};

		//Push in a pool
		m_chunkPool.push_back(std::make_pair(pos,std::weak_ptr<ChunkType>(ptr)));
		m_chunkPool.back();

		return ptr;
	}

	template<class ChunkType>
	inline std::shared_ptr<ChunkType> ChunkLoader<ChunkType>::loadChunk(intcord2 pos)
	{
		std::ifstream inputChunk(m_prefix + "chunk"+ std::to_string(pos.x)+"a"+ std::to_string(pos.y) + m_postfix, std::ios::binary);
		if (!inputChunk.is_open()) {
			LOGCATEGORY("ChunkLoader/loadChunk").warn("Chunk is atemted to be loaded, but not exist! %s, %s, Chunk(%i, %i)", m_prefix.c_str(), m_postfix.c_str(), pos.x, pos.y);
			return std::shared_ptr<ChunkType>();
		}

		cereal::BinaryInputArchive ChunkArchive(inputChunk);


		auto chunk = std::make_shared<ChunkType>();
		ChunkArchive(chunk.get()->cords);
		ChunkArchive(*chunk.get());

		//Push in a pool
		m_chunkPool.push_back(std::make_pair(pos, std::weak_ptr<ChunkType>(chunk)));
		static_cast<ChunkBase*>(chunk.get())->m_deleteCallback = [this, chunkThis = chunk.get()]() {

			for (auto it = m_chunkPool.begin(); it != m_chunkPool.end(); it++)
			{
				if (it->first == chunkThis->cords) {
					m_chunkPool.erase(it);
					saveChunk(chunkThis);
					return;
				}
			}
			LOGCATEGORY("LAMBDA").error("Cant find deletee in a pool!");
		};

		return chunk;
	}

}