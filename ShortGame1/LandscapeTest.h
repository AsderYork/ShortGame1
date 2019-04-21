#pragma once
#include "Ogre_Service.h"

namespace GEMTest {

	class ActualTester;

	class LandscapeTest : public GEM::Service
	{		
	private:

		GEM::Ogre_Service* m_OgreService;
		ActualTester* m_tester;

	public:

		LandscapeTest(GEM::Ogre_Service* OgreService);
		virtual ActionResult initialize();
		virtual void shutdown() override
		{
		}
		virtual ActionResult preFrame(float timeDelta) override
		{
			return ActionResult::AR_OK;
		}
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