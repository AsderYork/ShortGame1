#pragma once
#include "LandscapeVisualization.h"
#include "Ogre_Service.h"
#include <GS_Client.h>
#include <map>

namespace GEM
{
	class LandscapeVisualizationController
	{
		GameSim::GS_Client* m_client;
		Ogre_Service* m_ogre;

		std::map<std::pair<int,int>,LandscapeVisualization> m_visualizations;

	public:
		inline LandscapeVisualizationController(GameSim::GS_Client* client, Ogre_Service* ogre) : m_client(client), m_ogre(ogre) {}

		void updateVisual();

		void Clear();
	};
}