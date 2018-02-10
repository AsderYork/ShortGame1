#include "stdafx.h"
#include "Server.h"

int main(int argc, char *argv[])
{
	GEM::Server server(4483, GEM::ServerData("First one!"));

	printf("Server is going to start right now!\n");
	server.Run();


	/*
	std::string Answer;
	std::cin >> Answer;

	while (Answer != "X")
	{
	Controller.RunALittle();

	auto ptr = Controller.GetNewConnectionIfAny();
	while (ptr != nullptr)
	{
	printf("New connection!\n");

	std::stringstream OutStre("Wellcome, newcomer!");
	ptr->Send(OutStre);
	ptr = Controller.GetNewConnectionIfAny();

	}


	std::stringstream OutStre(Answer);
	for (auto& Connection : Controller.ConnectionList())
	{
	Connection->Send(OutStre);

	auto RecStr = Connection->Recive().str();
	if (RecStr.size() != 0) { printf("Recived:%s\n", RecStr.c_str()); }
	Connection->Recive().str(std::string());
	}
	std::cin >> Answer;
	}
	*/
	return 0;
}