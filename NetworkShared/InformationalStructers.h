#pragma once
#include <cereal\cereal.hpp>

namespace GEM
{
	struct ServerData
	{
		std::string ServerName;

		ServerData() {};
		ServerData(std::string name) : ServerName(name) {}

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(ServerName); // serialize things by passing them to the archive
		}

	};

	struct ClientData
	{
		std::string ClientName;

		ClientData() {};
		ClientData(std::string name) : ClientName(name) {}

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(ClientName); // serialize things by passing them to the archive
		}
	};
}