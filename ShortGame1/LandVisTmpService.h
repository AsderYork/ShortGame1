#pragma once
#include "EngineController.h"
#include "LandscapeVisualization.h"

namespace GEM
{
	class LandVisTmpService : public Service
	{
	private:
		LandscapeVisualization m_landVis;
	public:
		// Унаследовано через Service
		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(float timeDelta) override;
		virtual ActionResult frame(float timeDelta) override;
		virtual ActionResult postFrame(float timeDelta) override;

		LandVisTmpService(Ogre_Service* OgreService, GameSim::LandscapeMeshGenerator* Chunk) :
			m_landVis(OgreService, Chunk) {}
	};
}