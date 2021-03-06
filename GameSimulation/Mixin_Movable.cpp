#include "Mixin_Movable.h"
#include <cereal\archives\binary.hpp>
#include "GameSimulation.h"

namespace GEM::GameSim
{


	void Mixin_Movable::SetPosition(float x, float y, float z)
	{
		m_pos = btVector3(x,y,z);
	}
	void Mixin_Movable::SetPosition(const btVector3 pos)
	{
		m_pos = pos;
	}
	void Mixin_Movable::SetVelocity(float X, float Y, float Z)
	{
		btVector3 NewVel = btVector3(X, Y, Z);
		//LOGCATEGORY("SETVELOCITY").info("Velocity changed from(%f,%f,%f) to (%f,%f,%f)", m_velocity.x(), m_velocity.y(), m_velocity.z(),X,Z,Y);
		m_velocity = NewVel;
	}
	void Mixin_Movable::SetVelocity(const btVector3 vel)
	{
		//LOGCATEGORY("SETVELOCITY").info("V_Velocity changed from(%f,%f,%f) to (%f,%f,%f)", m_velocity.x(), m_velocity.y(), m_velocity.z(), vel.x(), vel.y(), vel.z());
		m_velocity = vel;
	}

	bool Mixin_Movable::tick(const GameTime delta)
	{		

		auto PassedSeconds = GameTimeToSeconds(delta);
		m_orientation *= m_rotation.slerp(btQuaternion(0.0f, 0.0f, 0.0f), PassedSeconds);
		auto RotatedVelocity = quatRotate(m_orientation, m_velocity);
		if (!RotatedVelocity.fuzzyZero())
		{
			RotatedVelocity = RotatedVelocity.normalize() * m_speed;
		}
		auto NewPos = m_pos + RotatedVelocity * PassedSeconds;
		
		
		auto Responce = Mixin_Movable_Singleton::Instance().m_landPhys->RayTest(NewPos + btVector3(0.0f, 256.0f, 0.0f), NewPos - btVector3(0.0f, 1.0f, 0.0f));
		if (!Responce) { return true; }

		SetPosition(*Responce);

		return true;
	}
	bool Mixin_Movable::NeedsUpdate()
	{
		if((std::chrono::system_clock::now() - m_lastUpdate) > std::chrono::milliseconds(500))
		{
			return true;
		}
		if (m_velocity.length2() != 0 || m_rotation.getAngle() != 0)
		{
			m_keepUpdating = true;
			return true;
		}
		else
		{
			if (m_keepUpdating) { m_keepUpdating = false; return true; }
		}

		return false;
	}
	void Mixin_Movable::SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason)
	{
		if(reason == UpdateReason::REGULAR){
			m_lastUpdate = std::chrono::system_clock::now();
		}

		archive(m_pos.x(), m_pos.y(), m_pos.z());
		archive(m_velocity.x(), m_velocity.y(), m_velocity.z());
		archive(m_orientation.x(), m_orientation.y(), m_orientation.z(), m_orientation.w());
		archive(m_rotation.x(), m_rotation.y(), m_rotation.z(), m_rotation.w());
	}

	bool Mixin_Movable::CheckAndReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime UpdateLag)
	{
		btVector3 NewPos;
		btScalar posx, posy, posz;
		archive(posx, posy, posz);
		NewPos.setValue(posx, posy, posz);

		btVector3 NewVel;
		btScalar vex, vey, vez;
		archive(vex, vey, vez);
		NewVel.setValue(vex, vey, vez);


		btScalar orx, ory, orz, orw;
		archive(orx, ory, orz, orw);
		btQuaternion NewOrient(orx, ory, orz, orw);


		btScalar Rotx, Roty, Rotz, Rotw;
		archive(Rotx, Roty, Rotz, Rotw);
		btQuaternion NewRot(Rotx, Roty, Rotz, Rotw);



		if (NewVel == m_velocity)
		{			
			if ((m_pos - (NewPos + (NewVel * GameTimeToSeconds(UpdateLag)))).length2() < (NewVel.length2() / 1000)) { return true; }
		}


		auto Responce = Mixin_Movable_Singleton::Instance().m_landPhys->RayTest(NewPos + btVector3(0.0f, 256.0f, 0.0f), NewPos - btVector3(0.0f, 1.0f, 0.0f));
		if (!Responce) { return true; }

		float MaxVelocity = 1.0f;
		MaxVelocity *= 1.01f;//float calculatons are not perfect so we account for some error
		if (NewVel.length2() > MaxVelocity*MaxVelocity)
		{//Velocity is bigger then the allowed maximum
			return false;
		}

		

		SetPosition(*Responce);
		SetVelocity(NewVel);
		m_orientation = NewOrient;
		m_rotation = NewRot;

		return true;


	}
	
	void Mixin_Movable::ApplyUpdate(cereal::BinaryInputArchive& archive)
	{
		btScalar posx, posy, posz;
		archive(posx, posy, posz);
		SetPosition(posx, posy, posz);

		btScalar vex, vey, vez;
		archive(vex, vey, vez);
		SetVelocity(vex, vey, vez);


		btScalar orx, ory, orz, orw;
		archive(orx, ory, orz, orw);
		m_orientation = btQuaternion(orx, ory, orz, orw);

		btScalar Rotx, Roty, Rotz, Rotw;
		archive(Rotx, Roty, Rotz, Rotw);
		m_rotation = btQuaternion(Rotx, Roty, Rotz, Rotw);

	}
	
}
