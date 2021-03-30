#pragma once
#include "Ogre_Service.h"

namespace GEMTest {

	class innerLandscape3Teseter;

	class Landscape3Test : public GEM::Service
	{
	private:

		GEM::Ogre_Service* m_OgreService;
		innerLandscape3Teseter* m_tester;

	public:

		Landscape3Test(GEM::Ogre_Service* OgreService, GEM::SDL_Controller* sdlController);
		virtual ActionResult initialize();
		virtual void shutdown() override
		{
		}
		virtual ActionResult preFrame(float timeDelta);
		virtual ActionResult frame(float timeDelta) override
		{
			return ActionResult::AR_OK;
		}
		virtual ActionResult postFrame(float timeDelta) override
		{
			return ActionResult::AR_OK;
		}
	};

}