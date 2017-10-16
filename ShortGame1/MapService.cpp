#include "stdafx.h"
#include "MapService.h"
#include "NodesToMC.h"

namespace GEM
{
	Service::ActionResult MapService::initialize()
	{
		GEM::ChunkLoader<GEM::NodeChunk> loader("../Map/", ".map");
		GEM::NodesToMCGeneratorController Generator(&loader);
		Generator.GenerateFromScratch(0, 0, m_ogreService);

		return ActionResult();
	}

	void GEM::MapService::shutdown()
	{
	}

	Service::ActionResult MapService::preFrame(double timeDelta)
	{
		return ActionResult();
	}

	Service::ActionResult MapService::frame(double timeDelta)
	{
		return ActionResult();
	}

	Service::ActionResult MapService::postFrame(double timeDelta)
	{
		return ActionResult();
	}
}
