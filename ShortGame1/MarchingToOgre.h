#pragma once
#include "Ogre_Service.h"
#include "SDL2_Service.h"
#include "MarchingCubes.h"
#include "EngineController.h"
#include "MarchingCubesNodeController.h"
#include <string>
#include "MarchingCubesNodeController.h"
#include "CEGUI_Service.h"

namespace GEM
{
	class MarchingToOgre : public Service, public SDL_MouseListener, public SDL_KeyboardListener
	{
	public:
		/**
		draws a map of MarchingCubes, using mesh, assessed by MeshName
		*/
		MarchingToOgre(std::string MeshName, Ogre_Service* OgreService, MarchingCubiesMeshCalc* Calc, SDL_Controller* SDLController, CEGUI_Service* CEGUI_Service, float MapScale, float MeshScale);

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
		MarchingCubeController* m_MenuOverlay;

		float m_mapScale;
		float m_meshScale;
		std::string m_meshName;
		Ogre_Service* m_ogreService;
		MarchingCubiesMeshCalc* m_calc;
		SDL_Controller* m_sdlController;
		CEGUI_Service* m_ceguiService;
		std::vector<Ogre::Item*> m_itemsVector;
		std::vector<Ogre::SceneNode*> m_nodesVector;
		Ogre::Item* m_SelectionCountur;
		Ogre::SceneNode* m_SelectionCounturNode;

		Ogre::Item* m_MarchingCubesItem = nullptr;
		Ogre::SceneNode* m_MarchingCubeNode = nullptr;

		bool m_ShowNodes = true;

		/*
		Querries should not be performed before all bounding boxes are set up.
		It is expected, that they will be set, after all the initialization. That's why this flag exists;
		*/
		bool m_initialized = false;

		/**
		For precise controll of node value and over parameters, menu is needed. But it's hard to work with it, when camera is moving along.
		When menu is active, camera movement is desabled and overlay became active;
		*/
		bool m_showMenu = false;

		/**
		performs all menu popping in and out. Must be called to change menu state;
		*/
		void ChangeStateOfMenu();


		// Унаследовано через SDL_MouseListener
		virtual void mouseMoved(const SDL_Event & arg) override;
		virtual void mousePressed(const SDL_MouseButtonEvent & arg) override;
		virtual void mouseReleased(const SDL_MouseButtonEvent & arg) override;

		// Унаследовано через SDL_KeyboardListener
		virtual void textInput(const SDL_TextInputEvent & arg) override;
		virtual void keyPressed(const SDL_KeyboardEvent & arg) override;
		virtual void keyReleased(const SDL_KeyboardEvent & arg) override;
	};
}