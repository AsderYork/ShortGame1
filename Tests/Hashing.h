#pragma once
#include "stdafx.h"
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include <boost/test/unit_test.hpp>

#include <Hasher.h>


BOOST_AUTO_TEST_SUITE(Helper)
BOOST_AUTO_TEST_CASE(int32_10_Constexpr)
{
	int32_t Arr[10] = 
	{
		Helper::Hasher<int32_t>::Hash("Oh no","CollisionSpace"),
		Helper::Hasher<int32_t>::Hash("They've","CollisionSpace"),


		Helper::Hasher<int32_t>::Hash("killed","CollisionSpace"),
		Helper::Hasher<int32_t>::Hash("Kenny","CollisionSpace"),


		Helper::Hasher<int32_t>::Hash("Bastards!","CollisionSpace"),
		Helper::Hasher<int32_t>::Hash("Or not?","CollisionSpace"),


		Helper::Hasher<int32_t>::Hash("Who knows?","CollisionSpace"),
		Helper::Hasher<int32_t>::Hash("This philosophy","CollisionSpace"),


		Helper::Hasher<int32_t>::Hash("is not a simple","CollisionSpace"),
		Helper::Hasher<int32_t>::Hash("thing, you know?","CollisionSpace"),
	};

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (i == j) { continue; }
			BOOST_TEST_REQUIRE(Arr[i] != Arr[j]);			
		}
	}

}


BOOST_AUTO_TEST_SUITE_END()