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

#include <LandscapeMeshGenerator.h>
#include "LandscapeVisualization.h"

#include <LandscapeChunkPack.h>

class TmpService : public GEM::Service
{
public:
	struct LandMesh
	{
		GEM::LandscapeVisualMesh vm;
		GEM::GameSim::LandscapeMesh *lm;
		std::pair<int, int> pos;

		LandMesh(GEM::GameSim::LandscapeMesh *_lm, std::pair<int, int> p ) : lm(_lm), pos(p) {}

		void Generate()
		{
			vm = GEM::LandscapeVisualization::GenerateVisualMesh(lm, pos);
		}
	};


	std::vector<LandMesh> Lands;
	GEM::LandscapeVisualMesh Cube;
	GEM::Ogre_Service* ogs;
	bool done = false;

	virtual ActionResult initialize() override
	{
		for (auto& q : Lands) { q.Generate(); }
		Cube = GEM::LandscapeVisualization::DoCube();
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

auto LandscapeTest = [](GEM::GameSim::LandscapeChunk * NewChunk)
{
	auto[ActualX, ActualZ] = NewChunk->getPosition();

	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			int ThisX = (ActualZ * 16) + x;
			int ThisZ = (ActualX * 16) + z;
			float NodeHeight = 20 + sin(ThisX / 3.0f) * 2 + sin(ThisZ / 3.83f + 1)*1.43f;

			for (int y = 0; y < (int)floor(NodeHeight); y++)
			{
				if (NewChunk->getNode(z, y, x).Value != 255) {
					printf("Chunk[%i;%i] differs in position (%i;%i;%i); Have %i but should %i!\n", ActualX, ActualZ, x, y, z, NewChunk->getNode(z, y, x).Value, 255);
				}
			}

			if (NewChunk->getNode(z, (int)floor(NodeHeight), x).Value != (int)(255 * (NodeHeight - floor(NodeHeight)))) {
				printf("Chunk[%i;%i] differs in position (%i;%i;%i); Have %i but should %i!\n", ActualX, ActualZ, x, (int)floor(NodeHeight), z, NewChunk->getNode(z, (int)floor(NodeHeight), x).Value, (int)(255 * (NodeHeight - floor(NodeHeight))));
			}
		}
	}
};

void LocalChunkTests(GEM::EngineController& Controller, GEM::Ogre_Service* OgreController)
{
	auto MakePlane = [](GEM::GameSim::LandscapeChunk& LC, int Height = 1)
	{
		for (int y = 0; y < Height; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					LC.getNode(z, y, x).Value = 255;
				}
			}
		}
	};

	auto MakeTrough = [](GEM::GameSim::LandscapeChunk& LC, int Height = 20)
	{
		for (int y = 0; y < Height; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					LC.getNode(x, y, z).Value = 255;
				}
			}
		}


		for (int x = 0; x < 16; x++)
		{
			for (int z = 6; z < 9; z++)
			{
				LC.getNode(x, Height - 1, z).Value = 0;
			}
		}

	};

	auto MakePyramid = [](GEM::GameSim::LandscapeChunk& LC, int Height = 10, int sz = 4)
	{
		for (int y = 0; y < Height; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					LC.getNode(z, y, x).Value = 255;
				}
			}
		}

		int y = Height;
		for (int q = sz; q >= 0; q--)
		{
			for (int x = 8 - q; x <= 8 + q; x++)
			{
				for (int z = 8 - q; z <= 8 + q; z++)
				{
					LC.getNode(z, y, x).Value = 255;
				}
			}
			y++;
		}


	};


	auto MakeHarmonics = [](GEM::GameSim::LandscapeChunk& LC, int Height = 10, float factor = 4.0f)
	{
		auto[ActualX, ActualZ] = LC.getPosition();

		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				int ThisX = (ActualZ * 16) + x;
				int ThisZ = (ActualX * 16) + z;
				float NodeHeight = 20 + sin(ThisX / 3.0f) * 2 + sin(ThisZ / 3.83f + 1)*1.43f;

				for (int y = 0; y < (int)floor(NodeHeight); y++)
				{
					LC.getNode(z, y, x).Value = 255;
				}
				LC.getNode(z, (int)floor(NodeHeight), x).Value = (int)(255 * (NodeHeight - floor(NodeHeight)));

			}
		}


	};

	auto FillChunk = [](GEM::GameSim::LandscapeChunk& LC)
	{
		/*for (int y = 0; y < 64; y++)
		{
		for (int x = 0; x < 16; x++)
		{
		for (int z = 0; z < 4; z++)
		{
		if( y < 12 + x)
		{
		LC.getNode(x, y, z).Value = 255;
		}
		else
		{
		LC.getNode(x, y, z).Value = 0;
		}
		}
		}
		}
		*/
		int bx = 0, by = 0, bz = 0;

		/*LC.getNode(bx + 0, by + 0, bz + 0).Value = 255;//0
		LC.getNode(bx + 1, by + 0, bz + 0).Value = 255;//1
		LC.getNode(bx + 0, by + 0, bz + 1).Value = 255;//2
		LC.getNode(bx + 1, by + 0, bz + 1).Value = 255;//3
		LC.getNode(bx + 0, by + 1, bz + 0).Value = 255;//4
		LC.getNode(bx + 1, by + 1, bz + 0).Value = 0;//5
		LC.getNode(bx + 0, by + 1, bz + 1).Value = 255;//6
		LC.getNode(bx + 1, by + 1, bz + 1).Value = 0;//7
		*/

		LC.getNode(bx + 0, by + 0, bz + 0).Value = 255;//0

	};


	static GEM::GameSim::LandscapeChunk LC02(0, 2), LC12(1, 2), LC22(2, 2);
	static GEM::GameSim::LandscapeChunk LC01(0, 1), LC11(1, 1), LC21(2, 1);
	static GEM::GameSim::LandscapeChunk LC00(0, 0), LC10(1, 0), LC20(2, 0);




	MakeHarmonics(LC00); MakeHarmonics(LC01); MakeHarmonics(LC02);
	MakeHarmonics(LC10); MakeHarmonics(LC11); MakeHarmonics(LC12);
	MakeHarmonics(LC20); MakeHarmonics(LC21); MakeHarmonics(LC22);

	auto TmpSe = Controller.AddService<TmpService>();
	TmpSe->ogs = OgreController;


	static auto LNDSCPM00 = GEM::GameSim::LandscapeMeshGenerator::Generate(&LC00, &LC01, &LC10, &LC11);
	static auto LNDSCPM01 = GEM::GameSim::LandscapeMeshGenerator::Generate(&LC01, &LC02, &LC11, &LC12);
	static auto LNDSCPM10 = GEM::GameSim::LandscapeMeshGenerator::Generate(&LC10, &LC11, &LC20, &LC21);
	static auto LNDSCPM11 = GEM::GameSim::LandscapeMeshGenerator::Generate(&LC11, &LC12, &LC21, &LC22);

	TmpSe->Lands.emplace_back(&LNDSCPM00, LC00.getPosition());
	TmpSe->Lands.emplace_back(&LNDSCPM01, LC01.getPosition());
	TmpSe->Lands.emplace_back(&LNDSCPM10, LC10.getPosition());
	TmpSe->Lands.emplace_back(&LNDSCPM11, LC11.getPosition());

}

int main(int argc, char *argv[])
{
	SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT));


	
	


	
	
	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
	auto NetworkController = Controller.AddService<GEM::NetworkController>();

	//LocalChunkTests(Controller, OgreController);


	auto ScreenController = Controller.AddService<GEM::ScreenController>(SDLController);
	auto GameSimService = Controller.AddService<GEM::GameSimController>(NetworkController);





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

	try
	{
		return Controller.start();
	}
	catch (std::exception &e)
	{
		LOGCATEGORY("Main").crit("Exception reached main! Exception:%s", e.what());
	}
	catch (...)
	{
		LOGCATEGORY("Main").crit("Exception reached main! And it's not std::excpetion!");
	}
	return 0;
}

