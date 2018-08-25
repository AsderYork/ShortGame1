#include "stdafx.h"
#include "FirstPersonGameCamera.h"
#include <GameSimulation.h>
#include <LogHelper.h>
#include <OGRE/OgreRoot.h>

namespace GEM
{

	void FirstPersonGameCamera::UpdateCamera()
	{
		if (m_entity.expired()) { return; }
		auto LockedEnt = m_entity.lock();
		auto Movable = static_cast<GameSim::Mixin_Movable*>(LockedEnt->GetMixinByID(GameSim::Mixin_Movable::MixinID));
		
		m_camera = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance")->getCameras()[0];

		auto Pos = Movable->getPos();
		auto Rot = Movable->getOrientation();

		btVector3 Shift(4, 2, 0);

		Shift.rotate(Rot.getAxis(), Rot.getAngle());
		Pos += Shift;

		//Shift Camera sligtly, so that object would remain visible
		m_camera->setPosition(Pos.x(), Pos.y(), Pos.z());
		m_camera->setOrientation(Ogre::Quaternion(Rot.getW(), Rot.getX(), Rot.getY(), Rot.getZ()));
	}

	void FirstPersonGameCamera::TieCamera(GameSim::ENTITY_ID_TYPE entID, GameSim::GameSimulation* gs)
	{
		m_entity = gs->m_entities.GetEntity(entID);
		if (m_entity.expired())
		{
			LOGCATEGORY("Camera\TieCamera").error("Can't tie camera to entity(#%i). There is no such entity!", entID);
			return;
		}

		auto Movable = static_cast<GameSim::Mixin_Movable*>(m_entity.lock()->GetMixinByID(GameSim::Mixin_Movable::MixinID));
		if (Movable == nullptr)
		{
			LOGCATEGORY("Camera\TieCamera").error("Can't tie camera to entity(#%i). This entity doesn't have a movable mixin!", entID);
			return;
		}

		UpdateCamera();
	}
}