#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "MarchingCubes.h"
#include "MarchingToOgre.h"
#include "CEGUI_Service.h"
#include "FPS_Layout.h"



int main(int argc, char *argv[])
{
	GEM::MarchingCubesCalculator Calc;


	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto MarchingVis = Controller.AddService<GEM::MarchingToOgre>("Cube_d.mesh", OgreController, &Calc, 6, 1);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);

	SDLController->registerMouseListener(MarchingVis);
	SDLController->registerKeyboardListener(MarchingVis);

	CEGUIController->AddLayout<GEM::FPS_Layout>(1);

    return Controller.start();
}

