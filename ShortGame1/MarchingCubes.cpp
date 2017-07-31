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

		m_map[1][1][1] = 255;
		m_map[1][2][1] = 255;
		m_map[1][1][2] = 255;
		m_map[1][2][2] = 255;

		m_map[2][1][1] = 255;
		m_map[2][2][1] = 255;
		m_map[2][1][2] = 255;
		m_map[2][2][2] = 255;
	}
	const std::vector<std::vector<std::vector<MarchingCubesCalculator::VoxelValue>>> MarchingCubesCalculator::getMap()
	{
		return m_map;
	}
	void MarchingCubesCalculator::setValueOfNode(int x, int y, int z, VoxelValue value)
	{
		m_map[x][y][z] = value;
	}
	int MarchingCubesCalculator::getValueOfNode(int x, int y, int z)
	{
		return m_map[x][y][z];
	}
}