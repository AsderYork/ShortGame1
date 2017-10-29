#include "stdafx.h"
#include "NTMCG_Naive.h"

#include <OGRE\OgreVector3.h>

namespace GEM
{
	const int NodeToMCGeneratorNaive::triTable[256][16] =
	{ { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
	{ 8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1 },
	{ 3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1 },
	{ 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
	{ 4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1 },
	{ 9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1 },
	{ 10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1 },
	{ 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
	{ 5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1 },
	{ 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1 },
	{ 2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
	{ 7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
	{ 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1 },
	{ 11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1 },
	{ 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1 },
	{ 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1 },
	{ 11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1 },
	{ 2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1 },
	{ 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
	{ 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1 },
	{ 6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
	{ 6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1 },
	{ 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1 },
	{ 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1 },
	{ 3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
	{ 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1 },
	{ 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1 },
	{ 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
	{ 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1 },
	{ 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1 },
	{ 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1 },
	{ 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
	{ 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
	{ 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1 },
	{ 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1 },
	{ 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
	{ 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1 },
	{ 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1 },
	{ 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1 },
	{ 3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
	{ 6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1 },
	{ 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1 },
	{ 10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
	{ 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1 },
	{ 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
	{ 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
	{ 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1 },
	{ 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1 },
	{ 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1 },
	{ 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1 },
	{ 0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1 },
	{ 7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
	{ 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1 },
	{ 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1 },
	{ 10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1 },
	{ 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1 },
	{ 7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
	{ 6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
	{ 8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1 },
	{ 6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1 },
	{ 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1 },
	{ 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1 },
	{ 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1 },
	{ 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
	{ 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1 },
	{ 10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1 },
	{ 10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
	{ 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1 },
	{ 9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
	{ 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1 },
	{ 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1 },
	{ 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1 },
	{ 7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1 },
	{ 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1 },
	{ 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1 },
	{ 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1 },
	{ 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1 },
	{ 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1 },
	{ 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1 },
	{ 6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1 },
	{ 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1 },
	{ 6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1 },
	{ 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1 },
	{ 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1 },
	{ 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1 },
	{ 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1 },
	{ 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1 },
	{ 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1 },
	{ 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1 },
	{ 0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1 },
	{ 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1 },
	{ 11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1 },
	{ 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1 },
	{ 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1 },
	{ 2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
	{ 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1 },
	{ 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1 },
	{ 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1 },
	{ 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
	{ 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1 },
	{ 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1 },
	{ 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1 },
	{ 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1 },
	{ 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1 },
	{ 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1 },
	{ 9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1 },
	{ 5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1 },
	{ 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1 },
	{ 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1 },
	{ 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1 },
	{ 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1 },
	{ 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1 },
	{ 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1 },
	{ 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1 },
	{ 11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
	{ 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1 },
	{ 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1 },
	{ 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1 },
	{ 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1 },
	{ 1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1 },
	{ 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1 },
	{ 0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
	{ 9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1 },
	{ 1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } };

	
	NodeToMCGeneratorNaive::MidPointBase & NodeToMCGeneratorNaive::CalcMidPoint(int CubeX, int CubeY, int CubeZ, int EdgeID)
	{
		/*
		For Every variant
		1)Get 2 nodes, which values will be used
		2)Fill 2 of 3 node cordinates, that is not affected by shift
		3)Fill affected cordinate, using LERP over values and positions of the adjacent nodes.
		4)Return MidPoint
		*/
		switch (EdgeID)
		{
		case 0: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 0);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 1);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Right.pos.x = (float)CubeX + midval;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Right.pos.y = (float)CubeY;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Right.pos.z = (float)CubeZ;

			return NodeEnvelopeVec[CubeX][CubeY][CubeZ].Right;
		}

		case 1: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 1);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 2);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Back.pos.x = (float)CubeX + 1;
			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Back.pos.y = (float)CubeY;
			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Back.pos.z = (float)CubeZ + midval;

			return NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Back;
		}

		case 2: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 3);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 2);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Right.pos.x = (float)CubeX + midval;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Right.pos.y = (float)CubeY;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Right.pos.z = (float)CubeZ + 1;

			return NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Right;
		}

		case 3: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 0);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 3);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Back.pos.x = (float)CubeX;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Back.pos.y = (float)CubeY;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Back.pos.z = (float)CubeZ + midval;

			return NodeEnvelopeVec[CubeX][CubeY][CubeZ].Back;
		}

		case 4: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 4);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 5);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Right.pos.x = (float)CubeX + midval;
			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Right.pos.y = (float)CubeY + 1;
			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Right.pos.z = (float)CubeZ;

			return NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Right;
		}

		case 5: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 5);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 6);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX + 1][CubeY + 1][CubeZ].Back.pos.x = (float)CubeX + 1;
			NodeEnvelopeVec[CubeX + 1][CubeY + 1][CubeZ].Back.pos.y = (float)CubeY + 1;
			NodeEnvelopeVec[CubeX + 1][CubeY + 1][CubeZ].Back.pos.z = (float)CubeZ + midval;

			return NodeEnvelopeVec[CubeX + 1][CubeY + 1][CubeZ].Back;
		}

		case 6: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 7);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 6);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ + 1].Right.pos.x = (float)CubeX + midval;
			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ + 1].Right.pos.y = (float)CubeY + 1;
			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ + 1].Right.pos.z = (float)CubeZ + 1;

			return NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ + 1].Right;
		}

		case 7: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 4);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 7);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"


			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Back.pos.x = (float)CubeX;
			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Back.pos.y = (float)CubeY + 1;
			NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Back.pos.z = (float)CubeZ + midval;

			return NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Back;
		}

		case 8: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 0);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 4);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Top.pos.x = (float)CubeX;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Top.pos.y = (float)CubeY + midval;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ].Top.pos.z = (float)CubeZ;

			return NodeEnvelopeVec[CubeX][CubeY][CubeZ].Top;
		}

		case 9: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 1);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 5);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Top.pos.x = (float)CubeX + 1;
			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Top.pos.y = (float)CubeY + midval;
			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Top.pos.z = (float)CubeZ;

			return NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Top;
		}

		case 10: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 2);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 6);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ + 1].Top.pos.x = (float)CubeX + 1;
			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ + 1].Top.pos.y = (float)CubeY + midval;
			NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ + 1].Top.pos.z = (float)CubeZ + 1;

			return NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ + 1].Top;
		}

		case 11: {
			auto N1 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 3);
			auto N2 = GetNodeAsCubie(CubeX, CubeY, CubeZ, 7);
			float midval = ((float)N1.Value + (float)N2.Value) / 512;//512 is the double of maximum values of "Value"

			NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Top.pos.x = (float)CubeX;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Top.pos.y = (float)CubeY + midval;
			NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Top.pos.z = (float)CubeZ + 1;

			return NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Top;
		}

		default: {assert(false);/*There is only 12 edges!*/}
		}
	}
	void NodeToMCGeneratorNaive::CreateCube(int CubeX, int CubeY, int CubeZ)
	{
		/**!
		This is a list of changes beeng made to this method and the resulting avg time.

		Vector filling shut down, list filling shut down - 0.47s
		"Vertices" became static and is cleared every time - 0.23s
		"Vertices" now do recive Vertices and flavors - 0.24s
		List is now being filld, if at least one Vertice is added - 0.33s
		CubeData is beeng filled up - 1.38s
		CubeData is now pre-reserve space in Generate - 1.16s
		CubeData is now being populated only if at least one Vertiex is used - 0.40s
		"Vertices" and iterator are now moved in, insted of a copy - 0.33s
		Move is now repeated in CubeData constructor - 0.43s
		It doens't work that way. So CubeData is now being populated every tieme, but with default constructor, if there is no Vertices - 0.77s
		All the CubeData are now creted in Generate. Their values just moved in them without construction - 0.63s
		CubeData now contains only iterators on List. And list contains vertexVector  - 0.43s
		*/

		auto cubeval = GetCubieValue(CubeX, CubeY, CubeZ);

		
		for (int i = 0; triTable[cubeval][i] != -1;i += 3) {
			auto& p1 = CalcMidPoint(CubeX, CubeY, CubeZ, triTable[cubeval][i]);
			auto& p2 = CalcMidPoint(CubeX, CubeY, CubeZ, triTable[cubeval][i + 1]);
			auto& p3 = CalcMidPoint(CubeX, CubeY, CubeZ, triTable[cubeval][i + 2]);

			//Used to ease access to these points
			MidPointBase* Ps[3] = { &p1, &p2, &p3 };

			//Calculate normals			
			auto& From1To2 = p1.pos - p2.pos;
			auto& From1To3 = p1.pos - p3.pos;

			Ogre::Vector3 Normal = From1To2.crossProduct(From1To3);
			Normal.normalise();
			for (auto& p : Ps)
			{
				p->normal += Normal;
				p->textcord1 = p->pos;
			}

			//Check for UpDown flavor

			//If flavor is UpDown

			float VorValResY = abs(90 - Normal.angleBetween(Ogre::Vector3(0, 1, 0)).valueDegrees());
			float ValResY = Normal.angleBetween(Ogre::Vector3(0, 1, 0)).valueDegrees();

			Flavor Flavor;
			if (abs(90 - Normal.angleBetween(Ogre::Vector3(0, 1, 0)).valueDegrees()) >= 30)
			{
				Flavor = Flavor::FLAVOR_UPDOWN;
			}
			//if flavor is LeftRight
			else if (abs(90 - Normal.angleBetween(Ogre::Vector3(1, 0, 0)).valueDegrees()) > 45)
			{
				Flavor = Flavor::FLAVOR_LEFTRIGHT;
			}
			//Then it's FrontBack
			else
			{
				Flavor = Flavor::FLAVOR_FRONTBACK;
			}

			for (auto& p : Ps)
			{
				m_vertices.push_back(std::make_pair(p, Flavor));				
			}

		}

	
		if (!m_vertices.empty()) {
			auto It = m_actuallyUsedCubesVertices.insert(m_actuallyUsedCubesVertices.end(), std::move(m_vertices));

			m_cubeData[XYZToLinearCube(CubeX, CubeY, CubeZ)].PosInActiveList = std::move(It);
			m_cubeData[XYZToLinearCube(CubeX, CubeY, CubeZ)].isSet = true;
			m_cubeData[XYZToLinearCube(CubeX, CubeY, CubeZ)].lastCubeVal = cubeval;
		}

		m_vertices.clear();
	}

	void NodeToMCGeneratorNaive::UpdateCube(int CubeX, int CubeY, int CubeZ)
	{
		auto& Er = m_cubeData[XYZToLinearCube(CubeX, CubeY, CubeZ)];
		//Given cube might be empty. In that case there's nothing to erase
		if (Er.isSet)
		{
			RemoveNormals(CubeX, CubeY, CubeZ, Er.lastCubeVal);
			m_actuallyUsedCubesVertices.erase(Er.PosInActiveList);
			Er.lastCubeVal = 0;
		}

		//Mark as unset in case if CreateCube will leave this cube empty
		Er.isSet = false;

		CreateCube(CubeX, CubeY, CubeZ);
	}


	void NodeToMCGeneratorNaive::RemoveNormals(int CubeX, int CubeY, int CubeZ, unsigned char cubeval)
	{
		for (int i = 0; triTable[cubeval][i] != -1;i += 3) {
			auto& p1 = CalcMidPoint(CubeX, CubeY, CubeZ, triTable[cubeval][i]);
			auto& p2 = CalcMidPoint(CubeX, CubeY, CubeZ, triTable[cubeval][i + 1]);
			auto& p3 = CalcMidPoint(CubeX, CubeY, CubeZ, triTable[cubeval][i + 2]);

			//Used to ease access to these points
			MidPointBase* Ps[3] = { &p1, &p2, &p3 };

			//Calculate normals			
			auto& From1To2 = p1.pos - p2.pos;
			auto& From1To3 = p1.pos - p3.pos;

			Ogre::Vector3 Normal = From1To2.crossProduct(From1To3);
			Normal.normalise();
			for (auto&p : Ps)
			{
				p->normal -= Normal;
			}

		}
	}

	void NodeToMCGeneratorNaive::Generate()
	{
		//Clear for regeneration
		VertexVector.clear();
		m_cubeData.clear();

		m_cubeData.resize(DimXZ*DimXZ*(DimY - 1));

		NodeEnvelopeVec.clear();
		regenerateEnvelope();
		m_actuallyUsedCubesVertices.clear();

		for (int x = 0; x < DimXZ; x++)
		{
			for (int y = 0; y < DimY - 1; y++)
			{
				for (int z = 0; z < DimXZ; z++)
				{
					CreateCube(x, y, z);
				}
			}
		}

		//Building Final Vertex&Index vectors stage
		for (auto UsedCube : m_actuallyUsedCubesVertices)
		{
			for (auto Vertex : UsedCube)
			{
				VertexVector.push_back(Vertex);
			}			
		}
	}

	void NodeToMCGeneratorNaive::Update()
	{
		/**
		Now we're trying to optimize Update.
		Start - 0.05s
		Removing all clears - 0.05s
		Remove Build lists stage - 0.0002s
		Remove only normalization - 0.047s
		Pre-reserve for some big number - 0.049s
		Remove vector's insertations - 0.026s

		*/
		VertexVector.clear();
		//For every changed cube
		for (auto& cubePos : ChangedCubies)
		{
			int X = cubePos / (DimXZ*(DimY - 1));
			int Y = (cubePos - (X*DimXZ*(DimY - 1))) / (DimXZ);
			int Z = (cubePos - (X*DimXZ*(DimY - 1))) - Y*(DimXZ);

			UpdateCube(X, Y, Z);
		}
		ChangedCubies.clear();

		
		//Building Final Vertex&Index vectors stage
		for (auto UsedCube : m_actuallyUsedCubesVertices)
		{
			for (auto Vertex : UsedCube)
			{
				VertexVector.push_back(Vertex);				
			}
		}
		
	}

	void NodeToMCGeneratorNaive::ChangeNode(int x, int y, int z)
	{
		if ((x - 1) >= 0)//Check that cubes to the left from the node are actally exist
		{
			if ((y - 1) >= 0)//Check that the cubes below the node do exist
			{
				if ((z - 1) >= 0)//Check that the cubes in front of the node do exist
				{
					int CubePos = XYZToLinearCube(x - 1, y - 1, z - 1);
					if (!std::binary_search(ChangedCubies.begin(), ChangedCubies.end(), CubePos))
					{
						ChangedCubies.push_back(CubePos);
						std::sort(ChangedCubies.begin(), ChangedCubies.end());
					}
				}
				if (z < DimXZ)
				{
					int CubePos = XYZToLinearCube(x - 1, y - 1, z);
					if (!std::binary_search(ChangedCubies.begin(), ChangedCubies.end(), CubePos))
					{
						ChangedCubies.push_back(CubePos);
						std::sort(ChangedCubies.begin(), ChangedCubies.end());
					}
				}
			}
			if (y  < DimY - 1)
			{
				if ((z - 1) >= 0)
				{
					int CubePos = XYZToLinearCube(x - 1, y, z - 1);
					if (!std::binary_search(ChangedCubies.begin(), ChangedCubies.end(), CubePos))
					{
						ChangedCubies.push_back(CubePos);
						std::sort(ChangedCubies.begin(), ChangedCubies.end());
					}
				}
				if (z < DimXZ)
				{
					int CubePos = XYZToLinearCube(x - 1, y, z);
					if (!std::binary_search(ChangedCubies.begin(), ChangedCubies.end(), CubePos))
					{
						ChangedCubies.push_back(CubePos);
						std::sort(ChangedCubies.begin(), ChangedCubies.end());
					}
				}
			}
		}
		if (x < DimXZ)//Check that cubes to the left from the node are actally exist
		{
			if ((y - 1) >= 0)//Check that the cubes below the node do exist
			{
				if ((z - 1) >= 0)//Check that the cubes in front of the node do exist
				{
					int CubePos = XYZToLinearCube(x, y - 1, z - 1);
					if (!std::binary_search(ChangedCubies.begin(), ChangedCubies.end(), CubePos))
					{
						ChangedCubies.push_back(CubePos);
						std::sort(ChangedCubies.begin(), ChangedCubies.end());
					}
				}
				if (z < DimXZ)
				{
					int CubePos = XYZToLinearCube(x, y - 1, z);
					if (!std::binary_search(ChangedCubies.begin(), ChangedCubies.end(), CubePos))
					{
						ChangedCubies.push_back(CubePos);
						std::sort(ChangedCubies.begin(), ChangedCubies.end());
					}
				}
			}
			if (y  < DimY - 1)
			{
				if ((z - 1) >= 0)
				{
					int CubePos = XYZToLinearCube(x, y, z - 1);
					if (!std::binary_search(ChangedCubies.begin(), ChangedCubies.end(), CubePos))
					{
						ChangedCubies.push_back(CubePos);
						std::sort(ChangedCubies.begin(), ChangedCubies.end());
					}
				}
				if (z < DimXZ)
				{
					int CubePos = XYZToLinearCube(x, y, z);
					if (!std::binary_search(ChangedCubies.begin(), ChangedCubies.end(), CubePos))
					{
						ChangedCubies.push_back(CubePos);
						std::sort(ChangedCubies.begin(), ChangedCubies.end());
					}
				}
			}
		}

	}

	std::pair<NodeToMCGeneratorNaive::MidPointBase*, NodeToMCGeneratorNaive::Flavor> NodeToMCGeneratorNaive::getVertexVectorElement(int i)
	{
		return VertexVector[i];
	}

	const int NodeToMCGeneratorNaive::getVertexVectorSize() const
	{
		return (int)VertexVector.size();
	}	
	const int NodeToMCGeneratorNaive::getIndexVectorSize() const
	{
		return getVertexVectorSize();
	}
	int NodeToMCGeneratorNaive::getIndexVectorElement(int i) const
	{
		return i;
	}


}