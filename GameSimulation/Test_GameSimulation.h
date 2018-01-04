#pragma once
#include "GameSimulation.h"
namespace GEM::Test
{
	bool test_()
	{

	}
	GEM::GameSim::GameSimulation GS(2);
	bool Succ1 = GS.AddPlayer(10, "Jesus");
	bool Succ4 = GS.AddPlayer(10, "Christ");
	bool Succ2 = GS.AddPlayer(12, "Rex");
	bool Succ3 = GS.AddPlayer(14, "Moses");
}