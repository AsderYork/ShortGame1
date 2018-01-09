#include "stdafx.h"
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include <boost/test/unit_test.hpp>

#include <Helper_UniqueInstanceGuarantee.h>

class TestClass : public GEM::Helper::UniqueInstanceHelper<TestClass, int>
{
private:
	static int MasterID;

	int UnqiueID;

public:
	TestClass() : UnqiueID(MasterID), GEM::Helper::UniqueInstanceHelper<TestClass, int>(MasterID) {
		MasterID++;
	}

	TestClass(int ID) : UnqiueID(ID), GEM::Helper::UniqueInstanceHelper<TestClass, int>(ID) {
	}

};
std::unordered_set<int> GEM::Helper::UniqueInstanceHelper<TestClass, int>::m_UnqiueProofSet;

int TestClass::MasterID = 0;


BOOST_AUTO_TEST_SUITE(Helper_SingleInstanceGuarantee)
BOOST_AUTO_TEST_CASE(JustCreateABunchOfInstances)
{
	auto testfunc = []() {TestClass TC1, TC2, TC3, TC4, TC5; };
	BOOST_CHECK_NO_THROW(testfunc());
}

BOOST_AUTO_TEST_CASE(CreateWithSameID)
{
	TestClass TC1(8);
	BOOST_CHECK_THROW(TestClass TC2(8), std::exception);
}

BOOST_AUTO_TEST_CASE(CreateAfterRemoval)
{
	{
	TestClass TC1(14);	
	}

	BOOST_CHECK_NO_THROW(TestClass TC1(14));
}


BOOST_AUTO_TEST_SUITE_END()