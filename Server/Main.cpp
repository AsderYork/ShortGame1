#include "stdafx.h"
#include "Server.h"

#include <sstream>
#include <vector>

#include <EventBase.h>
#include <EventSerializator.h>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>

#include <GamePhysics.h>
#include <LandscapeChunkController.h>

#include <TestPlace.h>

struct PreData
{
	std::string Mnstr;
	PreData() {}
	PreData(std::string b) : Mnstr(b) {}

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(x, y);
	}
};

struct Datastr
{
	std::vector<PreData> dat;
	
	template<class Archive>
	void save(Archive & archive) const
	{
		GEM::Helper::SaveVector<uint8_t>(archive,dat);
	}


	template<class Archive>
	void load(Archive & archive)
	{
		GEM::Helper::LoadVector<uint8_t>(archive, dat);
	}
	
};


int main(int argc, char *argv[])
{
	Datastr A;
	A.dat.emplace_back(23,13);


	std::stringstream stst;
	{
		cereal::BinaryOutputArchive ar(stst);
		ar(A);

	}


	auto sz = stst.str().size();
	Datastr B;
	{
		cereal::BinaryInputArchive ar(stst);
		ar(B);

	}


	//GEM::GameSim::DoPhysics();
	
	GEM::GameSim::LandscapeChunk LC1, LC2, LC3, LC4;
	LC1.getNode(4, 4, 4).Value = 100;

	GEM::GameSim::LandscapeMeshGenerator LNDSCPM(&LC1, &LC2, &LC3, &LC4);
	
	GEM::Server server(4483, GEM::ServerData("First one!"));

	printf("Server is going to start right now!\n");
	server.Run();


	return 0;
}