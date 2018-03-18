#include "stdafx.h"
#include "Server.h"

#include <sstream>
#include <vector>

#include <EventBase.h>
#include <EventSerializator.h>
#include <cereal\types\vector.hpp>

#include <GamePhysics.h>
#include <LandscapeChunkController.h>


int main(int argc, char *argv[])
{
	std::vector<int> a1{ 1,3,5,6,7 }, a2{ 1,2,3,4,5,8 };


	/*auto [dif1, dif2] = GEM::GameSim::GetArraysDifferences(a1.begin(), a1.end(), a2.begin(), a2.end());
	printf("Unique1:");
	for (auto& un : dif1)
	{
		printf("%i; ", un);
	}
	printf("\nUnique2:");
	for (auto& un : dif2)
	{
		printf("%i; ", un);
	}
	*/

	//GEM::GameSim::DoPhysics();
	
	GEM::GameSim::LandscapeChunk LC1, LC2, LC3, LC4;
	LC1.getNode(4, 4, 4).Value = 100;

	GEM::GameSim::LandscapeMeshGenerator LNDSCPM(&LC1, &LC2, &LC3, &LC4);
	
	GEM::Server server(4483, GEM::ServerData("First one!"));

	printf("Server is going to start right now!\n");
	server.Run();


	return 0;
}