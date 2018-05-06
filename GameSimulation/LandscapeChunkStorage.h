#pragma once
#include "LandscapeChunkStorageListener.h"
#include "LandscapeMeshGenerator.h"

namespace GEM::GameSim
{
	/**!
	Stores chunks. This thing is replacing a current system of storing chunks in dispatchers. It will provide
	uniform interface for chunk access and also a way to store aadditional info next to chunks to ease access to it
	when a chunk is processed.

	\tparam AdditionalDataPacks A list of all structures, that should be added as additional data next to a chunk.
	MUST be default-constructable.
	*/
	template<class... AdditionalDataPacks>
	class ChunkStorage
	{
	public:
		struct ChunkStruct
		{
			LandscapeChunk chunk;
			LandscapeMesh mesh;
			std::tuple<AdditionalDataPacks...> AdditionalData;
			//Turns out allmost every system needs, that before processing, chunk shuould also have
			//It's mesh avaliable.
			bool isProcessed = false;

			ChunkStruct(int x, int z) : chunk(x, z) {}
		};
	private:

		std::map<std::pair<int, int>, ChunkStruct> m_data;

		enum class CallAction { AddNewChunk, RemoveChunk };

		std::vector<std::function<void(ChunkStruct*, CallAction)>> m_listeners;


		inline void ListenerRoutine(ChunkStruct* Chunk, CallAction reason)
		{
			for (auto& listenerLambda : m_listeners)
			{
				listenerLambda(Chunk, reason);
			}
		}

	public:

		/**!
		Registers a listener.
		\tparam DataBlockOfInterest A type of block of per-chunk data, that will be passed to a
		*/
		template<typename DataBlockOfInterest>
		inline void RegisterListener(ChunkStorageListener<DataBlockOfInterest>* listener)
		{
			auto NewListenerLambda = [listener, this](ChunkStruct* chunk, CallAction action) {
				switch (action)
				{
				case CallAction::AddNewChunk:
				{listener->NewChunkAdded(&(chunk->chunk), &chunk->mesh, &std::get<DataBlockOfInterest>(chunk->AdditionalData)); break; }
				case CallAction::RemoveChunk:
				{listener->ChunkRemoved(&(chunk->chunk), &chunk->mesh, &std::get<DataBlockOfInterest>(chunk->AdditionalData)); break; }
				}
			};
			m_listeners.emplace_back(NewListenerLambda);
		}

		template<>
		inline void RegisterListener(ChunkStorageListener<void>* listener)
		{
			auto NewListenerLambda = [listener, this](ChunkStruct* chunk, CallAction action) {
				switch (action)
				{
				case CallAction::AddNewChunk:
				{listener->NewChunkAdded(&(chunk->chunk), &chunk->mesh); break; }
				case CallAction::RemoveChunk:
				{listener->ChunkRemoved(&(chunk->chunk&chunk->mesh)); break; }
				}
			};
			m_listeners.emplace_back(NewListenerLambda);
		}

		/**!
		Returns a ChunkStruct with a specified position. If there is no such chunk, nullptr will be returned.
		*/
		inline ChunkStruct * getChunk(int x, int z)
		{
			auto it = m_data.find(std::make_pair(x, z));
			if (it == m_data.end()) { return nullptr; }
			return &(it->second);
		}

		/**!
		Adds new chunks and returns a pointer to it.
		If a chunk with given coordinates allready exists, no new chunks will be added and a pointer to an
		existing one will be returned.
		\param[in] generatorFunc a function, that should fill chunk data before it will be passed to listeners
		This method won't trigger NewChunkAdded event! It requires a pass of generateMeshesForUnpreparedChunks
		and creation of a mesh for this event to be raised.
		*/
		inline ChunkStruct * addChunk(int x, int z, std::function<void(LandscapeChunk*)> generatorFunc)
		{
			auto it = m_data.find(std::make_pair(x, z));
			if (it != m_data.end()) { return &(it->second); }

			auto ptr = &(m_data.emplace(std::piecewise_construct, std::forward_as_tuple(x, z), std::forward_as_tuple(x, z)).first->second);
			generatorFunc(&(ptr->chunk));
			return ptr;
		}

		

		/**!
		Removes a chunk from storage.
		If there actually is a chunk with a specified position, ChunkRemoved will be called for every
		listener and then chunk will be removed from storage.
		If there is no chunk with specified position,
		*/
		inline void RemoveChunk(int x, int z, std::function<void(LandscapeChunk*)> generatorFunc)
		{
			auto it = m_data.find(std::make_pair(x, z));
			if (it == m_data.end()) { return; }

			generatorFunc(&(it->second.chunk));
			ListenerRoutine(&(it->second), CallAction::RemoveChunk);
			m_data.erase(it);
		}

		/**!
		Chunk is truly working only if it's mesh is also present. This method should be called
		after every succession of chunk creations, to build meshes for chunks, where it's possible.
		Only when mesh is build, NewChunkAdded of listenres will be called!
		
		\note to generate a mesh for chunk (x,y), chunks (x+1,y),(x,y+1) and (x+1,y+1) must also be in a storage!
		*/
		void generateMeshesForUnpreparedChunks()
		{
			for (auto& ch : m_data)
			{
				if (!ch.second.isProcessed)
				{
					auto[x, z] = ch.first;
					auto Forward = getChunk(x, z+1);
					auto Right = getChunk(x+1, z);
					auto ForwardRight = getChunk(x + 1, z+1);
					if ((Forward != nullptr) && (Right != nullptr) && (ForwardRight != nullptr))
					{
						ch.second.mesh = LandscapeMeshGenerator::Generate(&ch.second.chunk, &Forward->chunk, &Right->chunk, &ForwardRight->chunk);
						ch.second.isProcessed = true;
						ListenerRoutine(&ch.second, CallAction::AddNewChunk);
					}
				}
			}
		}

		inline ~ChunkStorage()
		{
			for (auto& ch : m_data)
			{
				ListenerRoutine(&(ch.second), CallAction::RemoveChunk);
			}
			m_data.clear();
		}

	};
}