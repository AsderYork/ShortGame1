#include "stdafx.h"
#include "GameSimService.h"
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>

namespace GEM
{
	void GameSimController::ActivateSimulation()
	{
		m_simmulationActive = true;
	}
	void GameSimController::DeactivateSimmulation()
	{
		Reset();
		m_simmulationActive = false;
	}
	Service::ActionResult GEM::GameSimController::initialize()
	{		
		return ActionResult::AR_OK;
	}

	void GEM::GameSimController::shutdown()
	{
	}

	Service::ActionResult GEM::GameSimController::preFrame(float timeDelta)
	{
		if (m_simmulationActive)
		{
			auto conn = m_network->getConnection();
			if (conn == nullptr)
			{
				DeactivateSimmulation();
				return ActionResult::AR_OK;
			}
			cereal::BinaryInputArchive ar(conn->Recive());
			if (!Tick(timeDelta, ar, conn->Recive().str().size() == 0))
			{
				DeactivateSimmulation();
				return ActionResult::AR_OK;
			}
		}
		return ActionResult::AR_OK;
	}

	Service::ActionResult GEM::GameSimController::frame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}

	Service::ActionResult GEM::GameSimController::postFrame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}
}