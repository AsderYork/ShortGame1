#include "stdafx.h"

#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "DebugLayout.h"
#include "DebugMapGraphicsService.h"
#include "ConsoleLayout.h"
#include "MapService.h"
#include "ScreenController.h"
#include "NetworkController.h"
#include "GameSimService.h"

#include "LoginScreen.h"
#include "GameVisualization.h"

#include <LandscapeMeshGenerator.h>
#include "LandVisTmpService.h"


//#include <GameSimulation.h>
//#include <Mixin_Controller.h>


int main(int argc, char *argv[])
{
	SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT));

	GEM::GameSim::LandscapeChunk LC01(0, 1), LC11(1, 1), LC21(2, 1);
	GEM::GameSim::LandscapeChunk LC00(0, 0), LC10(1, 0), LC20(2, 0);

	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			LC00.getNode(x, 0, z).Value = 255;
		}
	}

	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			LC10.getNode(x, 2, z).Value = 255;
		}
	}

	for (int x = 0; x < 16; x++)
	{
		LC10.getNode(x, 6, 8).Value = 255;
		LC00.getNode(x, 6, 8).Value = 255;
	}

	LC00.getNode(8, 6, 8).Value = 255;
	
	auto start = std::chrono::steady_clock::now();
	GEM::GameSim::LandscapeMeshGenerator LNDSCPM1(&LC00, &LC01, &LC10, &LC11);
	auto finish = std::chrono::steady_clock::now();
	printf("BuildTime:%14.8f\n", std::chrono::duration<float>(finish - start).count());
	GEM::GameSim::LandscapeMeshGenerator LNDSCPM2(&LC10, &LC11, &LC20, &LC21);
	
	
	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
	auto NetworkController = Controller.AddService<GEM::NetworkController>();

	auto LVTSController1 = Controller.AddService<GEM::LandVisTmpService>(OgreController, &LNDSCPM1);
	auto LVTSController2 = Controller.AddService<GEM::LandVisTmpService>(OgreController, &LNDSCPM2);

	//auto ScreenController = Controller.AddService<GEM::ScreenController>(SDLController);
	//auto GameSimService = Controller.AddService<GEM::GameSimController>(NetworkController);
	//auto GameVisualization = Controller.AddService<GEM::GameVisualization>(GameSimService, OgreController);





	//SDLController->registerMouseListener(DebugMapController);
	//CEGUIController->AddLayout<GEM::DebugLayout>(1, OgreController, MapService);
	//GEM::ConsoleLayout* Console = static_cast<GEM::ConsoleLayout*>(CEGUIController->AddLayout<GEM::ConsoleLayout>(SDLController));
	
	/*Console->getEvaluator().RegisterFunction("Map.SetDraw", GEM::Evaluator::OBJTYPE::UNDECLARED, MapService, &GEM::MapService::setDrawDistance);
	Console->getEvaluator().RegisterFunction("Map.GetDraw", GEM::Evaluator::OBJTYPE::NUM, MapService, &GEM::MapService::getDrawDistance);
	
	Console->getEvaluator().RegisterFunction("Map.SetPrep", GEM::Evaluator::OBJTYPE::UNDECLARED, MapService, &GEM::MapService::setPrepareDistance);
	Console->getEvaluator().RegisterFunction("Map.GetPrep", GEM::Evaluator::OBJTYPE::NUM, MapService, &GEM::MapService::getPrepareDistance);

	Console->getEvaluator().RegisterFunction("Eng.SetMaxFPS", GEM::Evaluator::OBJTYPE::UNDECLARED, &Controller, &GEM::EngineController::setMaximumFPS);
	*/


	//ScreenController->AddScreen<GEM::LoginScreen>(NetworkController, GameSimService);

    return Controller.start();
	return 0;
}

