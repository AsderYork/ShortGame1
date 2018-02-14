#include "stdafx.h"
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include <boost/test/unit_test.hpp>

#include <GS_PlayerController.h>


BOOST_AUTO_TEST_SUITE(PlayerController)
BOOST_AUTO_TEST_CASE(Create4Players)
{
	GEM::GameSim::PlayerController PC;
	auto P1 = PC.addPlayer(GEM::GameSim::Player("Jhon"));
	auto P2 = PC.addPlayer(GEM::GameSim::Player("Vladec"));
	auto P3 = PC.addPlayer(GEM::GameSim::Player("Suzeassa"));
	auto P4 = PC.addPlayer(GEM::GameSim::Player("Jokq"));

	BOOST_CHECK(PC.getPlayersVector().size() == 4);

	BOOST_CHECK(P1.has_value() == true);
	BOOST_CHECK(P2.has_value() == true);
	BOOST_CHECK(P3.has_value() == true);
	BOOST_CHECK(P4.has_value() == true);

	BOOST_CHECK(P1->get().playerName == "Jhon");
	BOOST_CHECK(P2->get().playerName == "Vladec");
	BOOST_CHECK(P3->get().playerName == "Suzeassa");
	BOOST_CHECK(P4->get().playerName == "Jokq");

}

BOOST_AUTO_TEST_CASE(RemoveMiddlePlayer)
{
	GEM::GameSim::PlayerController PC;
	auto P1 = PC.addPlayer(GEM::GameSim::Player("Jhon"));
	auto P2 = PC.addPlayer(GEM::GameSim::Player("Vladec"));
	auto P3 = PC.addPlayer(GEM::GameSim::Player("Suzeassa"));
	auto P4 = PC.addPlayer(GEM::GameSim::Player("Jokq"));

	BOOST_CHECK(P1->get().playerName == "Jhon");
	BOOST_CHECK(P2->get().playerName == "Vladec");
	BOOST_CHECK(P3->get().playerName == "Suzeassa");
	BOOST_CHECK(P4->get().playerName == "Jokq");

	PC.RemovePlayer(std::move(P2.value()));

	BOOST_CHECK(P1->get().playerName == "Jhon");
	BOOST_CHECK(P3->get().playerName == "Suzeassa");
	BOOST_CHECK(P4->get().playerName == "Jokq");


}

BOOST_AUTO_TEST_CASE(LostTickets)
{
	GEM::GameSim::PlayerController PC;
	auto P1 = PC.addPlayer(GEM::GameSim::Player("Jhon"));
	{
		auto P2 = PC.addPlayer(GEM::GameSim::Player("Vladec"));
		auto P3 = PC.addPlayer(GEM::GameSim::Player("Suzeassa"));
	}
	auto P4 = PC.addPlayer(GEM::GameSim::Player("Jokq"));

	BOOST_CHECK(P1->get().playerName == "Jhon");
	BOOST_CHECK(P4->get().playerName == "Jokq");

	PC.RemovePlayer(std::move(P1.value()));

	BOOST_CHECK(P4->get().playerName == "Jokq");


}



BOOST_AUTO_TEST_SUITE_END()