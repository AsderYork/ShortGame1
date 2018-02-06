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

#include <GameSimulation.h>
#include <GameSim_EventReciver.h>
*/

#include <Mixin_Health.h>

#include <TestPlace.h>
#include <sstream>

#define REGISTER_MIXIN_CLASS(classname) Mixin_Controller::Instance().RegisterMixinClass(classname::MixinID, #classname);

int main(int argc, char *argv[])
{
	using namespace GEM::GameSim;

	REGISTER_MIXIN_CLASS(Mixin_Movable);
	REGISTER_MIXIN_CLASS(Mixin_Health);
	Mixin_Controller::Instance().RegisterMethod(14, 1, Grab(&Mixin_Movable::Shift), "Shift");


	MixedEntity<Mixin_Movable, Mixin_Health> Ent1{ Mixin_Movable(), Mixin_Health(25) };


	MixedEntity<Mixin_Movable> Ent2{ Mixin_Movable()};

	//MixedEntity<Mixin_Health> Ent2{  };

	//std::tuple<Mixin_Health> Tup{ Mixin_Health(25,10) };

	auto& S = Ent1;
	S.tick(0.1f);

	auto GrabRes = Grab(&Mixin_Movable::Shift);
	auto GrabResHelth = Grab(&Mixin_Health::SetHealth);

	{
		std::stringstream ss;

		{
			cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

			int m1 = 12, m2 = 65, m3 = 4;
			oarchive(m1, m2, m3);
		}
		{
			cereal::BinaryInputArchive  oarchive(ss);

			GrabRes(&Ent1, oarchive);
		}

	}


	{
		std::stringstream ss;

		{
			cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

			float m1 = 13;
			oarchive(m1);
		}
		{
			cereal::BinaryInputArchive  oarchive(ss);

			GrabResHelth(&Ent1, oarchive);
		}

	}


	//Mixin_Controller::Instance().RegisterMixinClass(14, "Movable");
	//Mixin_Controller::Instance().RegisterMethod(14, 1, MIXIN_REG_ROUTINE(Mixin_Movable, Move), "Move");

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

