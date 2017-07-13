#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_WindowCreate.h"

void StopPoint()
{
	printf("And were done!'n");
}


int main(int argc, char *argv[])
{
	std::atexit(StopPoint);

	GEM::SDL_Controller SDLController;
	SDLController.MakeWindow();

	GEM::EngineController Controller;
	Controller.AddService<GEM::SDL_Controller>();
    return Controller.start();
}

