#include "stdafx.h"
#include "Server.h"

#include <sstream>
#include <vector>

#include <EventBase.h>
#include <EventSerializator.h>
#include <cereal\types\vector.hpp>

#include <GamePhysics.h>
#include <LandscapeChunkController.h>

struct Pospos
{
	float x = 0, y = 0, z = 0;
	btVector3 getPos() { return btVector3(x, y, z); }
};

int main(int argc, char *argv[])
{
	GEM::GameSim::LandscapeChunkController LCCC;

	Pospos pp1, pp2;
	auto N1 = LCCC.createNewLoader([&]() {return pp1.getPos(); });

	LCCC.ProcessChunks();
	LCCC.ProcessChunks();
	pp1.x += 16.0f;
	LCCC.ProcessChunks();
	LCCC.RemoveLoader(N1);
	LCCC.ProcessChunks();

	N1 = LCCC.createNewLoader([&]() {return pp1.getPos(); });
	LCCC.ProcessChunks();
	auto N2 = LCCC.createNewLoader([&]() {return pp2.getPos(); });
	LCCC.ProcessChunks();

	LCCC.RemoveLoader(N1);
	LCCC.ProcessChunks();

	//GEM::GameSim::DoPhysics();
	
	GEM::GameSim::LandscapeChunk LC1, LC2, LC3, LC4;
	LC1.getNode(4, 4, 4).Value = 100;

	GEM::GameSim::LandscapeMeshGenerator LNDSCPM(&LC1, &LC2, &LC3, &LC4);
	
	GEM::Server server(4483, GEM::ServerData("First one!"));

	printf("Server is going to start right now!\n");
	server.Run();


	return 0;
}