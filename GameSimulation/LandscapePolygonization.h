#pragma once
#include "LandscapeChunk.h"
#include <vector>

namespace GEM::GameSim
{
	/**
	Generates mesh based on landscape data
	*/
	class LandscapePolygonization
	{
	public:
		/**
		Generates mesh. Provided chunks must be arranged x-row first from lowest angle.
		Amount of provided chunks must be 2^detailLevel^2.
		 
		  12  13  14  15
		z  8  9   10  11
		^  4  5   6   7
		|  0  1   2   3
		|___\
			/x
		*/
		void Polygonize(std::vector<LandscapeChunk*> chunks, unsigned int dtailLevel);


	private:

	};


}