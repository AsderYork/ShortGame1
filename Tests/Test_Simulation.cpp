#include "stdafx.h"
#define BOOST_TEST_MODULE GameSimulation
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <boost/test/included/unit_test.hpp>

#include <GameSimulation.h>


BOOST_AUTO_TEST_CASE(Add_3_Players)
{
	GEM::GameSim::GameSimulation GS(3);
	
	bool Succ1 = GS.AddPlayer(10, "Jesus");
	bool Succ2 = GS.AddPlayer(12, "Rex");
	bool Succ3 = GS.AddPlayer(13, "Moses");
	
	BOOST_CHECK(Succ1);
	BOOST_CHECK(Succ2);
	BOOST_CHECK(Succ3);
}


BOOST_AUTO_TEST_CASE(Player_Overload)
{
	GEM::GameSim::GameSimulation GS(2);

	bool Succ1 = GS.AddPlayer(10, "Jesus");
	bool Succ2 = GS.AddPlayer(12, "Rex");
	bool Succ3 = GS.AddPlayer(13, "Moses");

	BOOST_CHECK(Succ1);
	BOOST_CHECK(Succ2);
	BOOST_CHECK(!Succ3);
}


BOOST_AUTO_TEST_CASE(Player_SameID)
{
	GEM::GameSim::GameSimulation GS(4);

	bool Succ1 = GS.AddPlayer(10, "Jesus");
	bool Succ2 = GS.AddPlayer(12, "Rex");
	bool Succ3 = GS.AddPlayer(12, "Moses");

	BOOST_CHECK(Succ1);
	BOOST_CHECK(Succ2);
	BOOST_CHECK(!Succ3);
}