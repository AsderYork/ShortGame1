#include "stdafx.h"
#include "MapService.h"
#include "NodesToMC.h"
#include <chrono>

namespace GEM
{
	Service::ActionResult MapService::initialize()
	{
		GEM::ChunkLoader<GEM::NodeChunk> loader("../Map/", ".map");
		GEM::NodesToMCGeneratorController Generator(&loader);

		auto start = std::chrono::steady_clock::now();
		Generator.GenerateFromScratch(0, 0, m_ogreService);
		Generator.GenerateFromScratch(1, 0, m_ogreService);


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
