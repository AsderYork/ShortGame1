#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "ChunkLoader.h"
#include "FPS_Layout.h"

struct ChunkTest : public GEM::ChunkBase {
public:
	
	int a=1;
	float b=2;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(a, b);
	}



	// Унаследовано через ChunkBase
	virtual void generateNewChunk() override
	{
		a = -getCords().x;
		b = -getCords().y;
	}
};

int main(int argc, char *argv[])
{
	GEM::ChunkLoader<ChunkTest> loader("../Map/", "");
	{
		auto Chunkerson = loader.getChunk(12, 43);
		auto Chunkersonder = loader.getChunk(12, 44);

		auto ChunkersonYonger = loader.getChunk(12, 43);
	}
	auto ChunkersonReincarnate = loader.getChunk(12, 43);
	
	loader.saveMagistral();

	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);
	auto CEGUIController = Controller.AddService<GEM::CEGUI_Service>(OgreController, SDLController);
//	auto MarchingVis = Controller.AddService<GEM::MarchingToOgre>("Cube_d.mesh", OgreController, VoxelMap.getMeshCalc(), SDLController, CEGUIController, 1, 0.1);

	CEGUIController->AddLayout<GEM::FPS_Layout>(1);

    return Controller.start();
}

