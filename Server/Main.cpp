#include "stdafx.h"
#include "Server.h"


int main(int argc, char *argv[])
{
	//GEM::GameSim::DoPhysics();
	
	GEM::Server server(4483, GEM::ServerData("First one!"));

	printf("Server is going to start right now!\n");
	server.Run();


	return 0;
}