#include "stdafx.h"
#include "GameSimService.h"
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>

namespace GEM
{
	void GameSimController::ActivateSimulation()
	{
		m_simmulationActive = true;
		auto maybePlayerID = m_network->getPlayerCharID();
		if (!maybePlayerID)
		{
			LOGCATEGORY("GameSimController/ActivateSimulation").error("Can't activate simulation! PlayerID is undefined. Network is probaly broken");
			DeactivateSimmulation();
		}
		else
		{
			m_playerCharacterID = maybePlayerID.value();
			m_updatesProcessor.AddControlledEntity(maybePlayerID.value());
		}

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
			std::stringstream OutStream;
			if (!Tick(timeDelta, ar, OutStream, conn->Recive().str().size() == 0))
			{
				DeactivateSimmulation();
				return ActionResult::AR_OK;
			}
			conn->Send(OutStream);
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