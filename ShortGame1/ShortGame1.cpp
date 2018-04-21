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
#include "LandscapeVisualization.h"

class TmpService : public GEM::Service
{
public:
	GEM::LandscapeVisualMesh vm1;
	GEM::LandscapeVisualMesh vm2;
	GEM::GameSim::LandscapeMesh *lm;
	GEM::Ogre_Service* ogs;
	std::pair<int, int> pos;
	bool done = false;

	virtual ActionResult initialize() override
	{

		vm2 = GEM::LandscapeVisualization::GenerateVisualMesh(lm, pos);
		vm1 = GEM::LandscapeVisualization::DoCube();
		return ActionResult::AR_OK;
	}
	virtual void shutdown() override
	{
	}
	virtual ActionResult preFrame(float timeDelta) override
	{
		return ActionResult::AR_OK;
	}
	virtual ActionResult frame(float timeDelta) override
	{
		return ActionResult::AR_OK;
	}
	virtual ActionResult postFrame(float timeDelta) override
	{
		return ActionResult::AR_OK;
	}
};

int main(int argc, char *argv[])
{
	SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT));

	/*GEM::GameSim::LandscapeChunk LC01(0, 1), LC11(1, 1), LC21(2, 1);
	GEM::GameSim::LandscapeChunk LC00(0, 0), LC10(1, 0), LC20(2, 0);

	auto FillChunk = [](GEM::GameSim::LandscapeChunk& LC)
	{
		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{					
					LC.getNode(x, y, z).Value = 255;
				}
			}
		}
	};

	FillChunk(LC00);
	FillChunk(LC01);
	FillChunk(LC10);
	FillChunk(LC11);
	*/
	/*for (int x = 0; x < 16; x++)
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

	LC00.getNode(10, 4, 8).Value = 255;
	LC00.getNode(10, 5, 8).Value = 255;
	LC00.getNode(10, 6, 8).Value = 255;

	LC00.getNode(10, 4, 9).Value = 255;
	LC00.getNode(10, 5, 9).Value = 255;
	LC00.getNode(10, 6, 9).Value = 255;

	LC00.getNode(11, 4, 8).Value = 255;
	LC00.getNode(11, 5, 8).Value = 255;
	LC00.getNode(11, 6, 8).Value = 255;

	LC00.getNode(11, 4, 9).Value = 255;
	LC00.getNode(11, 5, 9).Value = 255;
	LC00.getNode(11, 6, 9).Value = 255;
	*/

	/*auto LNDSCPM1 = GEM::GameSim::LandscapeMeshGenerator::Generate(&LC00, &LC01, &LC10, &LC11);
	GEM::LandscapeVisualMesh vm;
	*/
	
	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
	auto NetworkController = Controller.AddService<GEM::NetworkController>();
	/*auto TmpSe = Controller.AddService<TmpService>();
	TmpSe->pos = LC00.getPosition();
	TmpSe->lm = &LNDSCPM1;
	TmpSe->ogs = OgreController;*/


	auto ScreenController = Controller.AddService<GEM::ScreenController>(SDLController);
	auto GameSimService = Controller.AddService<GEM::GameSimController>(NetworkController);
	auto GameVisualization = Controller.AddService<GEM::GameVisualization>(GameSimService, OgreController);





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

