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
	while (_getch() != 'X')
	{
		Controller.RunALittle();
	}

    return 0;
}

