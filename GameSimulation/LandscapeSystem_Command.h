#pragma once

#include "NetworkCommandBase.h"
#include "LandscapeChunkPack.h"


#include "Helper_VariableSizeSerialization.h"

#include <vector>

namespace GEM::GameSim
{

	struct ChunkRequest
	{
		int x;
		int z;
		uint64_t version;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(x, z, version);
		}

		inline ChunkRequest() {};
		inline ChunkRequest(int _x, int _z, uint64_t _ver = 0) : x(_x), z(_z), version(_ver) {}

	};
	/**!
	When client connects to a server, or when it moves around, it might request some chunks from the server.
	To do that, it uses this command. It lists required chunks along with their last known version. If client
	have no previous version of required chunk, it must use value 0 for chunk version.
	*/
	class LandscapeSystemCommand_RequestChunks : public NetworkCommand
	{
	public:

		LandscapeSystemCommand_RequestChunks() : NetworkCommand(1) {}

		std::vector<ChunkRequest> requests;

		template<class Archive>
		void save(Archive & archive) const
		{
			
			Helper::SaveVector<uint16_t>(archive, requests);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			Helper::LoadVector<uint16_t>(archive, requests);
		}
	};

	/**!
	After client's request, server responds with this command.
	It must list all requested chunks. For every request chunk, if version is 0 then server
	have refused to provide chunk, if it's not zero, then there is also datapack that contains this chunk.
	*/
	class LandscapeSystemCommand_Responce : public NetworkCommand
	{
	public:
		std::vector<ChunkRequest> requests;
		LandscapeChunkPack pack;

		LandscapeSystemCommand_Responce() : NetworkCommand(1) {}


		template<class Archive>
		void save(Archive & archive) const
		{
			Helper::SaveVector<uint16_t>(archive, requests);
			archive(pack);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			Helper::LoadVector<uint16_t>(archive, requests);
			archive(pack);
		}
		
	};
}