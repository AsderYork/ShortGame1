#include "stdafx.h"
#include <boost/test/unit_test.hpp>

#include <GS_PlayerController.h>
BOOST_AUTO_TEST_SUITE(TestSimulation_Players)


BOOST_AUTO_TEST_CASE(Add_3_Players)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(Controller.AddPlayer(0,"Adam"));
	BOOST_CHECK(Controller.AddPlayer(2,"Jimmie"));
	BOOST_CHECK(Controller.AddPlayer(3, "Marie"));
	BOOST_CHECK_EQUAL(Controller.GetPlayerCount(),3);
}

BOOST_AUTO_TEST_CASE(SameID_Conflict)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(Controller.AddPlayer(0, "Vince"));
	BOOST_CHECK(!Controller.AddPlayer(0, "Jannie"));
	BOOST_CHECK_EQUAL(Controller.GetPlayerCount(), 1);
}

BOOST_AUTO_TEST_CASE(SameNames_Conflict)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(Controller.AddPlayer(0, "Jackue"));
	BOOST_CHECK(!Controller.AddPlayer(1, "Jackue"));
	BOOST_CHECK_EQUAL(Controller.GetPlayerCount(), 1);
}

BOOST_AUTO_TEST_CASE(AddAndRemovePlayer)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(Controller.AddPlayer(0, "Jackue"));
	BOOST_CHECK(Controller.RemovePlayer(0));
	BOOST_CHECK_EQUAL(Controller.GetPlayerCount(), 0);
}

BOOST_AUTO_TEST_CASE(RemoveNonExistantPlayer)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(!Controller.RemovePlayer(0));
	BOOST_CHECK_EQUAL(Controller.GetPlayerCount(), 0);
}

BOOST_AUTO_TEST_CASE(AddPlayerAfterRemoval)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(Controller.AddPlayer(0, "Jackue"));
	BOOST_CHECK(Controller.RemovePlayer(0));
	BOOST_CHECK(Controller.AddPlayer(0, "Milene"));
	BOOST_CHECK_EQUAL(Controller.GetPlayerCount(), 1);
}

BOOST_AUTO_TEST_CASE(GetPlayerIDByName)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(Controller.AddPlayer(43, "Jackue"));
	BOOST_CHECK_EQUAL((*Controller.GetIDbyName("Jackue")), 43);
}

BOOST_AUTO_TEST_CASE(GetPlayerIDForNonExistantPlayer)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(!Controller.GetIDbyName("Jackue").has_value());
}

BOOST_AUTO_TEST_CASE(GetPlayernameByID)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(Controller.AddPlayer(43, "Jackue"));
	BOOST_CHECK_EQUAL((*Controller.GetNameByID(43)), "Jackue");
}

BOOST_AUTO_TEST_CASE(GetPlayernameByIDForNonExistantPlayer)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	BOOST_CHECK(!Controller.GetNameByID(43).has_value());
}

BOOST_AUTO_TEST_CASE(UseEntityList)
{
	GEM::GameSim::GameSim_PlayerController Controller;
	Controller.AddPlayer(43, "Jackue");
	auto ListPtr = Controller.GetEntityListByID(43);
	ListPtr->insert(0);
	ListPtr->insert(4);
	BOOST_CHECK(Controller.GetEntityListByID(43)->find(4) != ListPtr->end());
}

BOOST_AUTO_TEST_SUITE_END()
