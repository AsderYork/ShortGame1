#pragma once
#include "EngineController.h"
#include "Ogre_Service.h"
#include "SDL2_Service.h"

namespace GEM
{
	class CEGUI_Service : public Service
	{
	public:
		CEGUI_Service(Ogre_Service* ogreService, SDL_Controller* sdlController);

		// Унаследовано через Service
		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(double timeDelta) override;
		virtual ActionResult frame(double timeDelta) override;
		virtual ActionResult postFrame(double timeDelta) override;
	private:
		Ogre_Service* m_ogreService;
		SDL_Controller* m_sdlController;
	};

}