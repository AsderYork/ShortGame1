#include "stdafx.h"
#include "MarchingCubes.h"


namespace GEM
{
	GEM::MarchingCubesCalculator::MarchingCubesCalculator()
	{
		m_map.resize(4);
		for (int i = 0; i < 4; i++)
		{
			m_map[i].resize(4);
			for (int j = 0; j < 4; j++)
			{
				m_map[i][j].resize(4);
				for (int k = 0; k < 4; k++)
				{
					m_map[i][j][k] = 0;
				}
			}
		}

		m_map[1][1][1] = 100;
		m_map[1][2][1] = 100;
		m_map[1][1][2] = 100;
		m_map[1][2][2] = 100;

		m_map[2][1][1] = 100;
		m_map[2][2][1] = 100;
		m_map[2][1][2] = 100;
		m_map[2][2][2] = 100;
	}
	const std::vector<std::vector<std::vector<MarchingCubesCalculator::VoxelValue>>> MarchingCubesCalculator::getMap()
	{
		return m_map;
	}
}