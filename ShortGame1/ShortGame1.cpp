#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "DebugLayout.h"
#include "DebugMapGraphicsService.h"


#include "NTMCG_Naive.h"
#include "NodesToMCGeneratorsTests.h"


#include "MapService.h"
#include <list>


int main(int argc, char *argv[])
{
	/*{
		GEM::ChunkLoader<GEM::NodeChunk> Loader("\test", "tst");
		{
			auto pt = Loader.getChunk(0, 0);
		}
	}
	*/

	//auto Res = GEM::TestSpeed<GEM::NodeToMCGeneratorNaive>(2, 5);
	//GEM::InterprateResults(Res);

	//auto ResU = GEM::TestSpeedUpdate<GEM::NodeToMCGeneratorNaive>(4, 10);
	//GEM::InterprateResults(ResU);


	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);

	auto MapService = Controller.AddService<GEM::MapService>(OgreController);
	auto DebugMapController = Controller.AddService<GEM::DebugMapGraphicsService>(OgreController, MapService);

	SDLController->registerMouseListener(DebugMapController);
	CEGUIController->AddLayout<GEM::DebugLayout>(1, OgreController, MapService);
		

    return Controller.start();
}

