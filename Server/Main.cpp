#include "stdafx.h"
#include "Server.h"

#include <sstream>
#include <vector>

#include <EventBase.h>
#include <EventSerializator.h>
#include <cereal\types\vector.hpp>

#include <TestPlace.h>

#include <LandscapeChunkPack.h>

int main(int argc, char *argv[])
{

	
	GEM::GameSim::LandscapeChunk LCH11;
	LCH11.Version = 112;
	LCH11.PosX = 1;
	LCH11.PosY = 2;
	LCH11.Nodes[12][12][12] = 15;

	GEM::GameSim::LandscapeChunk LCH12;
	LCH12.Version = 76;
	LCH12.PosX = 3;
	LCH12.PosY = 4;
	LCH12.Nodes[2][3][4] = 11;

	GEM::GameSim::LandscapeChunkPack LPack({ &LCH11 , &LCH12 });

	auto RetVec = LPack.UnpackLandscapeChunks();

	
	GEM::Server server(4483, GEM::ServerData("First one!"));

	printf("Server is going to start right now!\n");
	server.Run();


	return 0;
}