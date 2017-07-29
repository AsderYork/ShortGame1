#pragma once
#include <vector>

namespace GEM
{
	/**
	Marching cubes. Fun thing, actually. Here's the link to a http://paulbourke.net/geometry/polygonise/ source.
	*/
	class MarchingCubesCalculator
	{
	public:
		using VoxelValue = unsigned char;

		MarchingCubesCalculator();

		const std::vector<std::vector<std::vector<VoxelValue>>> getMap();
	private:
		std::vector<std::vector<std::vector<VoxelValue>>> m_map;
	};

}