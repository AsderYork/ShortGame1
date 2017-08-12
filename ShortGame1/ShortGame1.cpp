#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "ChunkLoader.h"
#include "FPS_Layout.h"

struct ChunkTest {
	int a=1;
	float b=2;
};

int main(int argc, char *argv[])
{
	GEM::ChunkLoader<ChunkTest> loader("../Map/", "");
	auto Loaded1 = loader.GetChunk(0, 0);
	

	//GEM::VoxelMap VoxelMap;
	//VoxelMap.SetPlayerPos(0, 0);

	
	//GEM::MarchingCubiesMeshCalc Calc;


	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
//	auto MarchingVis = Controller.AddService<GEM::MarchingToOgre>("Cube_d.mesh", OgreController, VoxelMap.getMeshCalc(), SDLController, CEGUIController, 1, 0.1);

	CEGUIController->AddLayout<GEM::FPS_Layout>(1);

    return Controller.start();
}

