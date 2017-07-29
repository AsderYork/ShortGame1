#pragma once
#include "Ogre_Service.h"
#include "MarchingCubes.h"
#include "EngineController.h"
#include <string>

namespace GEM
{
	class MarchingToOgre : public Service
	{
	public:
		/**
		draws a map of MarchingCubes, using mesh, assessed by MeshName
		*/
		MarchingToOgre(std::string MeshName, Ogre_Service* OgreService, MarchingCubesCalculator* Calc, int MapScale, int MeshScale);

		// Унаследовано через Service
		virtual ActionResult initialize();
		virtual void shutdown();
		virtual ActionResult preFrame(double timeDelta) override;
		virtual ActionResult frame(double timeDelta) override;
		virtual ActionResult postFrame(double timeDelta) override;
	private:
		int m_mapScale;
		int m_meshScale;
		std::string m_meshName;
		Ogre_Service* m_ogreService;
		MarchingCubesCalculator* m_calc;
		std::vector<Ogre::Item*> m_itemsVector;
		std::vector<Ogre::SceneNode*> m_nodesVector;
	};
}