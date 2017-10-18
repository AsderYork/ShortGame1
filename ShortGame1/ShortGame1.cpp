#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "DebugLayout.h"
#include "DebugMapGraphicsService.h"


#include "MapService.h"

int main(int argc, char *argv[])
{


	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);

	auto MapService = Controller.AddService<GEM::MapService>(OgreController);
	auto DebugMapController = Controller.AddService<GEM::DebugMapGraphicsService>(OgreController, MapService);

	SDLController->registerMouseListener(DebugMapController);
	CEGUIController->AddLayout<GEM::DebugLayout>(1, OgreController);

    return Controller.start();
}

