#include "LandscapeVisibilityManager.h"
#include <algorithm>

#include "Landscape3Common.h"

namespace GEM::GameSim {

	LandscapeVisibilityManager::MapViewerId LandscapeVisibilityManager::MapViewer::idGenerator = 0;
	

	LandscapeVisibilityManager::MapViewerId LandscapeVisibilityManager::AddMapViewer(b3Vector3 pos, float visibilityRange) {

		Viewers.emplace_back(pos, visibilityRange);
		auto id = Viewers.back().Id;

		std::sort(Viewers.begin(), Viewers.end());

		return id;

	}
	void LandscapeVisibilityManager::UpdateMapViewer(MapViewerId id, b3Vector3 pos)
	{
		std::find_if(Viewers.begin(), Viewers.end(), [&id](MapViewer x) {return x.Id == id; })->Pos = pos;
	}
	void LandscapeVisibilityManager::DeleteMapViewer(MapViewerId id)
	{
		Viewers.erase(std::find_if(Viewers.begin(), Viewers.end(), [&id](MapViewer x) {return x.Id == id; }));		
	}


	
	struct chunkWithFlags {
		b3Vector3 position;
		char condidatePosition;
		char confirmedPosition;
		int IntPosX, IntPosY, IntPosZ;

		bool IsLess(const int x, const int y, const int z) const {
			return IntPosX < x && IntPosY < y && IntPosZ < z;
		}
		bool IsEqual(const int x, const int y, const int z) const {
			return IntPosX == x && IntPosY == y && IntPosZ == z;
		}

		bool operator<(const chunkWithFlags& other) const {
			return IntPosX == other.IntPosX && IntPosY == other.IntPosY && IntPosZ == other.IntPosZ;
		}

		chunkWithFlags(int x, int y, int z, char condindate) :
			IntPosX(x),
			IntPosY(y),
			IntPosZ(z),
			condidatePosition(condindate)
		{

		}

	};

	/**
	Let's assume that all the chunks and so on are of powers of two and just look at a 1-dimisional case.
	Whe should aslo first look at a fixed-chunk-size case

	Let chunk size be 16, and player's position be x. Then floor(x / 16) will be player's chunk. and floor(x/16) +- L will be player's neghbourhood.
	Then all loaded chunks is just a unique sum of all player's neghbouring chunks.

	Leth there be 4 different chunk sizes. 16, 32, 64, 128. There is also a falloff for each of them L1 - distance from player, where chunks MUST 
	be at least 16. L2 where chunks must be at least 32, and so on. Obviously, L1 < L2 < L3 < L4;

	So Let's just do what we did before for 16x chunks. Then we'll do the same thing for L2, L3 and L4. Then the actualization step. We will go through
	all L1's and confirm that the're L1. Then We'll go thhrough all potential L2 chunks and greedyly assign all cubes of L2 chunks an L2 status.
	Then we'll do the same for all the rest chunks.



	*/
	void LandscapeVisibilityManager::Frame() {

		/*std::vector<chunkWithFlags> chunks;
		for (auto viewer : Viewers)
		{



		}*/

	}

}
