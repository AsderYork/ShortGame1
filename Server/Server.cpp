// Server.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <GameSimulation.h>
#include <Mixin_Controller.h>


#include "ConnectionController.h"

#include <windows.h>
#include <iostream>
#include <conio.h>

#include <cereal\cereal.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>



struct ServerInfo
{
	std::string ServerName;

	ServerInfo() {};
	ServerInfo(std::string Name) : ServerName(Name) {};

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(ServerName);
	}
};

class Server
{
private:
	GEM::ConnectionController m_controller;
	GEM::GameSim::GameSimulation m_gameSim;

	ServerInfo m_serverInfo;
public:
	
	

	Server(int Port, ServerInfo Serverinfo) : m_controller(Port), m_serverInfo(Serverinfo){}



	void handleNetwork()
	{
		m_controller.RunALittle();

		auto ptr = m_controller.GetNewConnectionIfAny();
		while (ptr != nullptr)
		{
			std::stringstream OutStre;
			cereal::BinaryInputArchive ar(OutStre);
			ar(m_serverInfo);
			printf("New connection!\n");
			ptr->Send(OutStre);
			ptr = m_controller.GetNewConnectionIfAny();
		}
	}

	
};


int main(int argc, char *argv[])
{
	
	

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

