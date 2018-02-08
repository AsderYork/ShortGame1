// Server.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "ConnectionController.h"

#include <windows.h>
#include <iostream>
#include <conio.h>

int main()
{





	GEM::ConnectionController Controller(5543);

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

    return 0;
}

