#include "stdafx.h"
#include "LandVisTmpService.h"

namespace GEM
{
	Service::ActionResult LandVisTmpService::initialize()
	{
		m_landVis.DoCube();
		m_landVis.GenerateMesh();
		return ActionResult::AR_OK;
	}

	void LandVisTmpService::shutdown()
	{
		m_landVis.UndoCube();
		m_landVis.RemoveMesh();
	}

	Service::ActionResult LandVisTmpService::preFrame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}

	Service::ActionResult LandVisTmpService::frame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}

	Service::ActionResult LandVisTmpService::postFrame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}
}
