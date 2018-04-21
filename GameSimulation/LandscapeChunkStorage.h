#pragma once
#include "LandscapeChunkStorageListener.h"

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
			std::tuple<AdditionalDataPacks...> AdditionalData;

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
				{listener->NewChunkAdded(&(chunk->chunk), &std::get<DataBlockOfInterest>(chunk->AdditionalData)); break; }
				case CallAction::RemoveChunk:
				{listener->ChunkRemoved(&(chunk->chunk), &std::get<DataBlockOfInterest>(chunk->AdditionalData)); break; }
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
				{listener->NewChunkAdded(&(chunk->chunk)); break; }
				case CallAction::RemoveChunk:
				{listener->ChunkRemoved(&(chunk->chunk)); break; }
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
		*/
		inline ChunkStruct * addChunk(int x, int z)
		{
			auto it = m_data.find(std::make_pair(x, z));
			if (it != m_data.end()) { return &(it->second); }

			auto ptr = &(m_data.emplace(std::piecewise_construct, std::forward_as_tuple(x, z), std::forward_as_tuple(x, z)).first->second);
			ListenerRoutine(ptr, CallAction::AddNewChunk);
			return ptr;
		}

		/**!
		Removes a chunk from storage.
		If there actually is a chunk with a specified position, ChunkRemoved will be called for every
		listener and then chunk will be removed from storage.
		If there is no chunk with specified position,
		*/
		inline void RemoveChunk(int x, int z)
		{
			auto it = m_data.find(std::make_pair(x, z));
			if (it == m_data.end()) { return; }

			ListenerRoutine(&(it->second), CallAction::RemoveChunk);
			m_data.erase(it);
		}

	};
}