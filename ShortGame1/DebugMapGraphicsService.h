#pragma once
#include "EngineController.h"
#include "Ogre_Service.h"
#include "SDL2_Service.h"
#include "MapService.h"

namespace GEM
{
	/**!
	Provides debug graphics for map system
	*/
	class DebugMapGraphicsService : public Service, public SDL_MouseListener
	{
	private:
		Ogre_Service* m_ogreService;
		MapService* m_mapService;
		Ogre::SceneNode* m_visionNode;

		/*Vision allows to see nodes in front of the camera*/
		float VisionSize=0;

		void CreateShader();
		
		enum Action{ACTION_NONE, ACTION_FILL, ACTION_DIG};
		Action m_action = ACTION_NONE;

	public:
		DebugMapGraphicsService(Ogre_Service* OgreService, MapService* MapService) :
			m_ogreService(OgreService),
			m_mapService(MapService)
		{}



		// Унаследовано через Service
		virtual ActionResult initialize() override;

		virtual void shutdown() override;

		virtual ActionResult preFrame(float timeDelta) override;

		virtual ActionResult frame(float timeDelta) override;

		virtual ActionResult postFrame(float timeDelta) override;


		//Inherited from SDL_MouseListener
		virtual void mouseMoved(const SDL_Event & arg) override;

		virtual void mousePressed(const SDL_MouseButtonEvent & arg) override;

		virtual void mouseReleased(const SDL_MouseButtonEvent & arg) override;

	};
}