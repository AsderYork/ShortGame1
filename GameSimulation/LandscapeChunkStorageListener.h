#pragma once
#include "LandscapeChunk.h"
#include "LandscapeMeshGenerator.h"

#include <map>
#include <vector>
#include <tuple>
#include <functional>

namespace GEM::GameSim
{
	/**!
	Registers in a ChunkStorage, accepting events of chunks going in and out of visibility,
	and also providing a way to add a layer of data in a chunk structure, to allow faster search
	*/
	template<typename StorageType>
	class ChunkStorageListener
	{
	public:
		/**!
		Called for every new chunk added in dispather, right after it filled with data
		\param[in] NewChunk a pointer to a chunk being added
		\note Whant to know chunk's position? It's in a chunk itself!
		*/
		virtual void NewChunkAdded(LandscapeChunk* NewChunk, LandscapeMesh* NewMesh, StorageType* SpecificData) = 0;

		/**!
		Called for every chunk right before it is removed.
		\param[in] NewChunk A chunk that will be removed.
		\note It is possible to perform cleanup of some added per-chunk stuff right here
		but it's adviced to do it in distructors of a data being added.
		*/
		virtual void ChunkRemoved(LandscapeChunk* NewChunk, LandscapeMesh* NewMesh, StorageType* SpecificData) = 0;
	};
	/**!
	ChunkStorageListener Specialization of a ChunkStorageListener with no specific storage type
	*/
	template<>
	class ChunkStorageListener<void>
	{
	public:
		/**!
		Called for every new chunk added in dispather.
		\param[in] NewChunk a pointer to a chunk being added
		\note Whant to know chunk's position? It's in a chunk itself!
		*/
		virtual void NewChunkAdded(LandscapeChunk* NewChunk) = 0;

		/**!
		Called for every chunk right before it is removed.
		\param[in] NewChunk A chunk that will be removed.
		\note It is possible to perform cleanup of some added per-chunk stuff right here
		but it's adviced to do it in distructors of a data being added.
		*/
		virtual void ChunkRemoved(LandscapeChunk* NewChunk) = 0;
	};

}