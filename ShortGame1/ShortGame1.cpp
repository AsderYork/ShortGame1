#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"


void StopPoint()
{
	printf("And were done!'n");
}


int main(int argc, char *argv[])
{
	
	
	std::atexit(StopPoint);

	GEM::EngineController Controller;
	Controller.AddService<GEM::SDL_Controller>();
	Controller.AddService<GEM::Ogre_Service>();
    return Controller.start();
}

