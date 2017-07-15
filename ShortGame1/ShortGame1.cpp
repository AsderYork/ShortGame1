#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"





int main(int argc, char *argv[])
{
	
	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
    return Controller.start();
}

