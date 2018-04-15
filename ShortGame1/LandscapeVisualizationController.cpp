#include "stdafx.h"
#include "LandscapeVisualizationController.h"

namespace GEM
{

	void LandscapeVisualizationController::updateVisual()
	{
		for (auto& chunk : m_client->m_chunkDispatcher.m_chunks)
		{
			if (chunk.mesh == nullptr) { continue; }
			auto it = m_visualizations.find(chunk.chunk.getPosition());
			if (it == m_visualizations.end())
			{
				auto newModel = m_visualizations.emplace(std::piecewise_construct, std::make_tuple(chunk.chunk.getPosition()), std::make_tuple(m_ogre, chunk.mesh.get()));
				newModel.first->second.GenerateMesh();
			}
		}
	}


	void LandscapeVisualizationController::Clear()
	{
		m_visualizations.clear();
	}
}