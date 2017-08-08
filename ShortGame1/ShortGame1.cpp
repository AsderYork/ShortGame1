#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "MarchingCubes.h"
#include "MarchingToOgre.h"
#include "CEGUI_Service.h"

#include "FPS_Layout.h"
#include "VoxelMap.h"
//#include "ChunkLoader.h"


int main(int argc, char *argv[])
{

	

	GEM::VoxelMap VoxelMap;
	VoxelMap.SetPlayerPos(0, 0);

	
	//GEM::MarchingCubiesMeshCalc Calc;


	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
	auto MarchingVis = Controller.AddService<GEM::MarchingToOgre>("Cube_d.mesh", OgreController, VoxelMap.getMeshCalc(), SDLController, CEGUIController, 1, 0.1);

	CEGUIController->AddLayout<GEM::FPS_Layout>(1);

    return Controller.start();
}

