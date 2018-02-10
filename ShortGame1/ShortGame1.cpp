#include "stdafx.h"

#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "DebugLayout.h"
#include "DebugMapGraphicsService.h"
#include "ConsoleLayout.h"
#include "MapService.h"



//#include <GameSimulation.h>
//#include <Mixin_Controller.h>

#include "GameClient.h"

int main(int argc, char *argv[])
{
	GEM::logHelper::setLog("laststart.log");
	SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT));

	GEM::GameClient GC(GEM::ClientData("Jasoz"));

	if (GC.Connect("127.0.0.1", 4483))
	{
		while (true)
		{
			if (!GC.process()) { break; }
		}
	}
	printf("So this is the end!\n");

	
	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);

	auto MapService = Controller.AddService<GEM::MapService>(OgreController);
	auto DebugMapController = Controller.AddService<GEM::DebugMapGraphicsService>(OgreController, MapService);

	MapService->setDrawDistance(1);
	MapService->setPrepareDistance(2);



	SDLController->registerMouseListener(DebugMapController);
	CEGUIController->AddLayout<GEM::DebugLayout>(1, OgreController, MapService);
	GEM::ConsoleLayout* Console = static_cast<GEM::ConsoleLayout*>(CEGUIController->AddLayout<GEM::ConsoleLayout>(SDLController));
	
	Console->getEvaluator().RegisterFunction("Map.SetDraw", GEM::Evaluator::OBJTYPE::UNDECLARED, MapService, &GEM::MapService::setDrawDistance);
	Console->getEvaluator().RegisterFunction("Map.GetDraw", GEM::Evaluator::OBJTYPE::NUM, MapService, &GEM::MapService::getDrawDistance);
	
	Console->getEvaluator().RegisterFunction("Map.SetPrep", GEM::Evaluator::OBJTYPE::UNDECLARED, MapService, &GEM::MapService::setPrepareDistance);
	Console->getEvaluator().RegisterFunction("Map.GetPrep", GEM::Evaluator::OBJTYPE::NUM, MapService, &GEM::MapService::getPrepareDistance);

	Console->getEvaluator().RegisterFunction("Eng.SetMaxFPS", GEM::Evaluator::OBJTYPE::UNDECLARED, &Controller, &GEM::EngineController::setMaximumFPS);

    return Controller.start();
	return 0;
}

