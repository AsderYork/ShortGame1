#include "stdafx.h"
#include <boost/test/unit_test.hpp>

#include <GS_EntityController.h>
BOOST_AUTO_TEST_SUITE(Entity_controller)

class TestEntity : public GEM::GameSim::Entity_Base
{
private:
	std::string m_SomeRandomInnerState;
public:
	TestEntity(std::string SRIS) : GEM::GameSim::Entity_Base(12),
		m_SomeRandomInnerState(SRIS)
	{}

	bool Functionality() { return true; }
};

class WrongClass
{
public:
	WrongClass(std::string SRIS) {}
	bool Functionality() { return false; }
};

BOOST_AUTO_TEST_CASE(UpAndRunning)
{
	GEM::GameSim::EntityController EC;
	auto ApplePtr = EC.AddNewEntity<WrongClass>(20, "I'm an apple!");

	BOOST_CHECK(ApplePtr->Functionality());
}

BOOST_AUTO_TEST_SUITE_END()