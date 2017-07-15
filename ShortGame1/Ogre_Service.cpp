#include "stdafx.h"
#include "Ogre_Service.h"

namespace GEM
{
	GEM::Ogre_Service::ActionResult GEM::Ogre_Service::initialize()
	{
		m_ogreLog.connectToOgreLog();

		m_root = std::make_unique<Ogre::Root>("plugins.cfg", "ogre.cfg");
		return ActionResult::AR_OK;
	}

	void GEM::Ogre_Service::shutdown()
	{
		m_root.release();
	}

	GEM::Ogre_Service::ActionResult GEM::Ogre_Service::preFrame(double timeDelta)
	{
		return ActionResult();
	}

	GEM::Ogre_Service::ActionResult GEM::Ogre_Service::frame(double timeDelta)
	{
		return ActionResult();
	}

	GEM::Ogre_Service::ActionResult GEM::Ogre_Service::postFrame(double timeDelta)
	{
		return ActionResult();
	}
}
