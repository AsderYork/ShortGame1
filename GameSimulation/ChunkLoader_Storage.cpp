#include "ChunkLoader_Storage.h"
#include "LandscapeChunkPack.h"
#include "LogHelper.h"

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>

#include <fstream>
#include <algorithm>

namespace GEM::GameSim
{
	std::optional<uint64_t> ChunkLoader_Storage::isChunkAvaliable(int x, int z)
	{
		auto SupposedRecord = MagistralRecord(x, z);

		auto& It = std::lower_bound(m_magistral.begin(), m_magistral.end(), SupposedRecord);
		if ((It != m_magistral.end()) && (*It == SupposedRecord))
		{
			return It->ChunkVersion;
		}
		
		return std::nullopt;
	}

	void ChunkLoader_Storage::LoadChunkIn(int x, int z, LandscapeChunk * target)
	{
		std::ifstream magistralFile(m_mapFolderPath + "\\x" + std::to_string(x)+"z" + std::to_string(x) + ".chunk", std::ios::binary);
		cereal::BinaryInputArchive archive(magistralFile);

		LandscapeChunkPack newChunkPack;
		archive(newChunkPack);

		auto NewChunk = newChunkPack.UnpackLandscapeChunks();

		(*target) = NewChunk[0];
	}

	void ChunkLoader_Storage::SaveChunk(LandscapeChunk * target)
	{
		auto[TargetPosX, TargetPosZ] = target->getPosition();
		auto& It = std::lower_bound(m_magistral.begin(), m_magistral.end(), MagistralRecord(TargetPosX, TargetPosZ));
		if ((It != m_magistral.end()) && (*It == MagistralRecord(TargetPosX, TargetPosZ)))
		{
			if (It->ChunkVersion == target->getVersion()) { return; }
		}
		else
		{
			m_magistral.insert(It, MagistralRecord(target->getVersion(), TargetPosX, TargetPosZ));
			isMagistralChanged = true;
		}
		std::ifstream magistralFile(m_mapFolderPath + "\\x" + std::to_string(TargetPosX) + "z" + std::to_string(TargetPosZ) + ".chunk", std::ios::binary);
		cereal::BinaryInputArchive archive(magistralFile);

		LandscapeChunkPack newChunkPack;
		archive(LandscapeChunkPack({target}));

		It->ChunkVersion = target->getVersion();
	}

	void ChunkLoader_Storage::Start(std::string MapFolderPath)
	{
		m_mapFolderPath = MapFolderPath;
		std::ifstream magistralFile(m_mapFolderPath + "\\magistral", std::ios::binary);
		cereal::BinaryInputArchive archive(magistralFile);

		try {
			archive(m_magistral);
		}
		catch (cereal::Exception&)
		{
			LOGCATEGORY("ChunkLoader/Start").info("Can't read magistral. It probably doesn't exist! So we just create it!");
			std::fstream NewMagistralFile;
			NewMagistralFile.open(m_mapFolderPath + "\\magistral", std::ios::out);
			NewMagistralFile.close();
		}
	}
	void ChunkLoader_Storage::SaveMagistral()
	{
		if (!isMagistralChanged) { return; }
		std::ofstream magistralFile(m_mapFolderPath + "\\magistral", std::ios::binary);
		cereal::BinaryOutputArchive archive(magistralFile);

		archive(m_magistral);
		isMagistralChanged = false;
	}
}
