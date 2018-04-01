#pragma once
#include "LandscapeChunk.h"

#include <string>
#include <optional>
#include <vector>

namespace GEM::GameSim
{
	class ChunkLoader_Storage
	{
	private:
		
		struct MagistralRecord
		{
			uint64_t ChunkVersion;
			int x, z;

			MagistralRecord(uint64_t _ChunkVersion, int _x, int _z) : ChunkVersion(_ChunkVersion),
				x(_x), z(_z) {}

			MagistralRecord(int _x, int _z) : ChunkVersion(0),
				x(_x), z(_z) {}

			MagistralRecord() : ChunkVersion(0),
				x(0), z(0) {}

			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(ChunkVersion, x, z);
			}

			friend bool operator<(const MagistralRecord& l, const MagistralRecord& r)
			{
				return std::tie(l.x, l.z) < std::tie(r.x, r.z);
			}

			friend bool operator==(const MagistralRecord& l, const MagistralRecord& r)
			{
				return std::tie(l.x, l.z) == std::tie(r.x, r.z);
			}

		};

		std::vector<MagistralRecord> m_magistral;
		std::string m_mapFolderPath;

	public:
		/**!
		Checks if chunk (x,z) is avaliable on a disk
		\returns Returns it's version if it's avaliable or returns nothing if it is not
		*/
		std::optional<uint64_t> isChunkAvaliable(int x, int z);

		/**!
		Loads chunk with specified position in a target place in memory
		
		\note Chunk might be unavaliable for loading, to check if chunk is actually avaliable,
		use \c isChunkAvaliable
		*/
		void LoadChunkIn(int x, int z, LandscapeChunk* target);

		/**
		Save chunk from memory to storage.
		\note this method is called for every chunk, that is leaving view, whcih a lot of chunks do.
		So it's recomended to check, if chunk really needs to be saved
		*/
		void SaveChunk(LandscapeChunk* target);

		/**!
		Prepares ChunkLoader for loading chunks
		\param[in] MapFolderPath Path to a folder, where map data is located
		*/
		void Start(std::string MapFolderPath);

		/**!
		Saves Magistral in storage.
		*/
		void SaveMagistral();
	};
}