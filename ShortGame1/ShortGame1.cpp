#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "FPS_Layout.h"


#include "NodeChunk.h"
#include "NodesToMC.h"

int main(int argc, char *argv[])
{
	GEM::ChunkLoader<GEM::NodeChunk> loader("../Map/", ".map");

	GEM::NodesToMCGeneratorController Generator(&loader);
	Generator.GenerateFromScratch(0, 0);

	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
//	auto MarchingVis = Controller.AddService<GEM::MarchingToOgre>("Cube_d.mesh", OgreController, VoxelMap.getMeshCalc(), SDLController, CEGUIController, 1, 0.1);

	CEGUIController->AddLayout<GEM::FPS_Layout>(1);

    return Controller.start();
}

