#include "stdafx.h"
#include "Server.h"

#include <sstream>
#include <vector>

#include <EventBase.h>
#include <EventSerializator.h>
#include <cereal\types\vector.hpp>

int main(int argc, char *argv[])
{
	/*std::stringstream s;

	{
		std::vector<std::unique_ptr<GEM::GameSim::EventBase>> vec;
		for (int i = 0; i < 43; i++)
		{
			vec.emplace_back(std::make_unique<GEM::GameSim::KeyboardEvent_PlayerJump>(i % 2 == 1));
		}

		cereal::BinaryOutputArchive ar(s);
		ar(vec);
	}
	auto i = s.str().size();
	{
		cereal::BinaryInputArchive ar(s);

		std::vector<std::unique_ptr<GEM::GameSim::EventBase>> vec;
		ar(vec);
		for (int i = 0; i < 43; i++)
		{
			if (!(static_cast<GEM::GameSim::KeyboardEventBase*>(vec[i].get())->m_isPressed == (i % 2 == 1)))
			{
				printf("error!\n");
			}
		}

		printf("no error!\n");
	}
	*/
	
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