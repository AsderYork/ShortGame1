#include "stdafx.h"
/*#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "DebugLayout.h"
#include "DebugMapGraphicsService.h"
#include "ConsoleLayout.h"
#include "MapService.h"
#include "NTMCG_TestSpeed.h"
*/
#include <GameSimulation.h>


#include <Mixin_Health.h>
#include <Mixin_Movable.h>
#include <Mixin_Controller.h>

//#include <TestPlace.h>
#include <sstream>

int main(int argc, char *argv[])
{
	using namespace GEM::GameSim;

	GameSimulation GSS;

	auto EntPtr = GSS.m_entities.AddNewEntity<MixedEntity<>>(1);
	auto MixEnt = GSS.m_entities.MixNewEntity<Mixin_Movable, Mixin_Health>(2, Mixin_Movable(), Mixin_Health(25));
	
	GSS.Tick(12);

	MixedEntity<Mixin_Movable, Mixin_Health> Ent1{ Mixin_Movable(), Mixin_Health(25) };


	MixedEntity<Mixin_Movable> Ent2{ Mixin_Movable()};

	Mixin_Controller::Instance().ApplyCommand(&Ent1, MixinCommandRetranslator(Mixin_Movable::MixinID, 1, -5, 3 ,6));

	Mixin_Controller::Instance().ApplyCommand(&Ent1, MixinCommandRetranslator(Mixin_Health::MixinID, 1, 18.0f));


	auto& S = Ent1;
	S.tick(0.1f);

	printf("Uas!\n");

	
	/*GEM::EngineController Controller;
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

    return Controller.start();*/
	return 0;
}

