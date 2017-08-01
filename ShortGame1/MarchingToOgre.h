#pragma once
#include "Ogre_Service.h"
#include "SDL2_Service.h"
#include "MarchingCubes.h"
#include "EngineController.h"
#include <string>

namespace GEM
{
	class MarchingToOgre : public Service, public SDL_MouseListener
	{
	public:
		/**
		draws a map of MarchingCubes, using mesh, assessed by MeshName
		*/
		MarchingToOgre(std::string MeshName, Ogre_Service* OgreService, MarchingCubesCalculator* Calc, int MapScale, int MeshScale);

		/**
		Changes the colour of meshes to match the values in map
		It is required, that size of map remains the same!
		*/
		void updatePrepresentation();

		// Унаследовано через Service
		virtual ActionResult initialize();
		virtual void shutdown();
		virtual ActionResult preFrame(double timeDelta) override;
		virtual ActionResult frame(double timeDelta) override;
		virtual ActionResult postFrame(double timeDelta) override;

	private:

		void CreateOrUpdateDatablock(int id, float value);
		void CreateSelectionDatablock();

		void CreateMarchingCubeDatablock();

		void CreateMesh();
		Ogre::IndexBufferPacked* createIndexBuffer();
	

		int m_mapScale;
		int m_meshScale;
		std::string m_meshName;
		Ogre_Service* m_ogreService;
		MarchingCubesCalculator* m_calc;
		std::vector<Ogre::Item*> m_itemsVector;
		std::vector<Ogre::SceneNode*> m_nodesVector;
		Ogre::Item* m_SelectionCountur;
		Ogre::SceneNode* m_SelectionCounturNode;

		Ogre::Item* m_MarchingCubesItem = nullptr;
		Ogre::SceneNode* m_MarchingCubeNode = nullptr;


		// Унаследовано через SDL_MouseListener
		virtual void mouseMoved(const SDL_Event & arg) override;
		virtual void mousePressed(const SDL_MouseButtonEvent & arg) override;
		virtual void mouseReleased(const SDL_MouseButtonEvent & arg) override;
	};
}