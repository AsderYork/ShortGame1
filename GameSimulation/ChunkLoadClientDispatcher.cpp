#include "ChunkLoadClientDispatcher.h"
#include "LandscapeSystem_Command.h"

#include "Mixin_Movable.h"

namespace GEM::GameSim
{

	ChunkLoadClientDispatcher::StoredChunk * ChunkLoadClientDispatcher::getChunk(int x, int z)
	{
		auto SearchLambda = [](const StoredChunk& chunk, std::pair<int, int> cord) {
			return chunk.chunk.getPosition() < cord;
		};

		auto SearchResult = std::lower_bound(m_chunks.begin(), m_chunks.end(), std::make_pair(x, z), SearchLambda);
		if (SearchResult == m_chunks.end())
		{
			return nullptr;
		}
		else if (SearchResult->chunk.getPosition() != std::make_pair(x, z))
		{
			return nullptr;
		}

		return &(*SearchResult);
	}

	ChunkLoadClientDispatcher::StoredChunk * ChunkLoadClientDispatcher::addNewChunk(int x, int z)
	{
		auto SearchLambda = [](const StoredChunk& chunk, std::pair<int, int> cord) {
			return chunk.chunk.getPosition() < cord;
		};

		auto SearchResult = std::lower_bound(m_chunks.begin(), m_chunks.end(), std::make_pair(x, z), SearchLambda);
		if (SearchResult == m_chunks.end())
		{
			SearchResult = m_chunks.emplace(SearchResult);
		}
		else if (SearchResult->chunk.getPosition() != std::make_pair(x, z))
		{
			SearchResult = m_chunks.emplace(SearchResult);
		}

		return &(*SearchResult);
	}


	void ChunkLoadClientDispatcher::FormRequest(ClientCommandDispatcher * MessageDispatcher)
	{
		auto& NewlyVisibleChunks = m_chunkController.getGlobalyNewlyVisibleChunks();

		if (NewlyVisibleChunks.size() == 0) { return; }

		LandscapeSystemCommand_RequestChunks RequestCommand;

		for (auto& visibleChunk : NewlyVisibleChunks)
		{
			RequestCommand.requests.emplace_back();
			RequestCommand.requests.back().x = visibleChunk.x;
			RequestCommand.requests.back().z = visibleChunk.z;

			if (m_chunkLoader.isChunkAvaliable(visibleChunk.x, visibleChunk.z))
			{
				auto ch = addNewChunk(visibleChunk.x, visibleChunk.z);
				m_chunkLoader.LoadChunkIn(visibleChunk.x, visibleChunk.z, &(ch->chunk));
				ch->isConfirmed = false;

				RequestCommand.requests.back().version = m_chunks.back().chunk.getVersion();
			}
			else
			{
				RequestCommand.requests.back().version = 0;
			}
		}

		MessageDispatcher->InsertPerformedCommand(std::make_unique<LandscapeSystemCommand_RequestChunks>(RequestCommand));
	}

	void ChunkLoadClientDispatcher::DoMeshes()
	{
		for (auto& visChunk : m_chunks)
		{
			if (visChunk.mesh != nullptr) { continue; }
			auto Cords = visChunk.chunk.getPosition();

			auto Front = &(getChunk(Cords.first + 1, Cords.second)->chunk);
			auto Right = &(getChunk(Cords.first, Cords.second + 1)->chunk);
			auto FrontRight = &(getChunk(Cords.first + 1, Cords.second + 1)->chunk);

			if ((Front != nullptr) && (Right != nullptr) && (FrontRight != nullptr))
			{
				visChunk.mesh = std::make_unique<LandscapeMeshGenerator>(&(visChunk.chunk), Front, Right, FrontRight);
			}
		}
	}


	void ChunkLoadClientDispatcher::Process(ClientCommandDispatcher* MessageDispatcher)
	{
		m_chunkController.ProcessChunks();
		FormRequest(MessageDispatcher);
		DoMeshes();
	}
}
