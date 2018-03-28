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



int main(int argc, char *argv[])
{
	//GEM::GameSim::DoPhysics();
	
	GEM::GameSim::LandscapeChunk LC1, LC2, LC3, LC4;
	LC1.getNode(4, 4, 4).Value = 100;

	GEM::GameSim::LandscapeMeshGenerator LNDSCPM(&LC1, &LC2, &LC3, &LC4);
	
	GEM::Server server(4483, GEM::ServerData("First one!"));

	printf("Server is going to start right now!\n");
	server.Run();


	return 0;
}