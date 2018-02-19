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



//#include <GameSimulation.h>
//#include <Mixin_Controller.h>


int main(int argc, char *argv[])
{
	SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT));
	
	
	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
	auto NetworkController = Controller.AddService<GEM::NetworkController>();
	auto ScreenController = Controller.AddService<GEM::ScreenController>(SDLController);
	auto GameSimService = Controller.AddService<GEM::GameSimController>(NetworkController);
	auto GameVisualization = Controller.AddService<GEM::GameVisualization>(GameSimService, OgreController);

	//auto MapService = Controller.AddService<GEM::MapService>(OgreController);
	//auto DebugMapController = Controller.AddService<GEM::DebugMapGraphicsService>(OgreController, MapService);

	//MapService->setDrawDistance(1);
	//MapService->setPrepareDistance(2);



	//SDLController->registerMouseListener(DebugMapController);
	//CEGUIController->AddLayout<GEM::DebugLayout>(1, OgreController, MapService);
	//GEM::ConsoleLayout* Console = static_cast<GEM::ConsoleLayout*>(CEGUIController->AddLayout<GEM::ConsoleLayout>(SDLController));
	
	/*Console->getEvaluator().RegisterFunction("Map.SetDraw", GEM::Evaluator::OBJTYPE::UNDECLARED, MapService, &GEM::MapService::setDrawDistance);
	Console->getEvaluator().RegisterFunction("Map.GetDraw", GEM::Evaluator::OBJTYPE::NUM, MapService, &GEM::MapService::getDrawDistance);
	
	Console->getEvaluator().RegisterFunction("Map.SetPrep", GEM::Evaluator::OBJTYPE::UNDECLARED, MapService, &GEM::MapService::setPrepareDistance);
	Console->getEvaluator().RegisterFunction("Map.GetPrep", GEM::Evaluator::OBJTYPE::NUM, MapService, &GEM::MapService::getPrepareDistance);

	Console->getEvaluator().RegisterFunction("Eng.SetMaxFPS", GEM::Evaluator::OBJTYPE::UNDECLARED, &Controller, &GEM::EngineController::setMaximumFPS);
	*/


	ScreenController->AddScreen<GEM::LoginScreen>(NetworkController, GameSimService);

    return Controller.start();
	return 0;
}

