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

	GEM::GameSim::LandscapeChunk LC1, LC2, LC3, LC4;
	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			LC1.getNode(x, 0, z).Value = 255;
		}
	}

	GEM::GameSim::LandscapeMeshGenerator LNDSCPM(&LC1, &LC2, &LC3, &LC4);
	
	
	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
	auto NetworkController = Controller.AddService<GEM::NetworkController>();

	auto LVTSController = Controller.AddService<GEM::LandVisTmpService>(OgreController, &LNDSCPM);

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

