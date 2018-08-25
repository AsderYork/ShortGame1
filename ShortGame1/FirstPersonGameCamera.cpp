#include "stdafx.h"
#include "FirstPersonGameCamera.h"
#include <GameSimulation.h>
#include <LogHelper.h>

namespace GEM
{
	void FirstPersonGameCamera::TieCamera(GameSim::ENTITY_ID_TYPE entID, GameSim::GameSimulation* gs)
	{
		auto Ent = gs->m_entities.GetEntity(entID);
		if (Ent.expired())
		{
			LOGCATEGORY("Camera\TieCamera").error("Can't tie camera to entity(#%i). There is no such entity!", entID);
			return;
		}

		auto Movable = static_cast<GameSim::Mixin_Movable*>(Ent.lock()->GetMixinByID(GameSim::Mixin_Movable::MixinID));
		if (Movable == nullptr)
		{
			LOGCATEGORY("Camera\TieCamera").error("Can't tie camera to entity(#%i). This entity doesn't have a movable mixin!", entID);
			return;
		}
	}
}