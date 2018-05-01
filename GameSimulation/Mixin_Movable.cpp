#include "Mixin_Movable.h"
#include "KeyboardEvents.h"
#include <cereal\archives\binary.hpp>
#include "GameSimulation.h"

namespace GEM::GameSim
{


	void Mixin_Movable::SetPosition(float x, float y, float z)
	{
		m_pos = btVector3(x,y,z);
	}
	void Mixin_Movable::SetPositionV(const btVector3 pos)
	{
		m_pos = pos;
	}
	void Mixin_Movable::SetVelocity(float X, float Y, float Z)
	{
		btVector3 NewVel = btVector3(X, Y, Z);
		//LOGCATEGORY("SETVELOCITY").info("Velocity changed from(%f,%f,%f) to (%f,%f,%f)", m_velocity.x(), m_velocity.y(), m_velocity.z(),X,Z,Y);
		m_velocity = NewVel;
	}
	void Mixin_Movable::SetVelocityV(const btVector3 vel)
	{
		//LOGCATEGORY("SETVELOCITY").info("V_Velocity changed from(%f,%f,%f) to (%f,%f,%f)", m_velocity.x(), m_velocity.y(), m_velocity.z(), vel.x(), vel.y(), vel.z());
		m_velocity = vel;
	}

	bool Mixin_Movable::tick(const GameTime delta)
	{
		auto NewPos = m_pos + m_velocity * GameTimeToSeconds(delta);
		
		auto Responce = Mixin_Movable_Singleton::Instance().m_landPhys->RayTest(NewPos + btVector3(0.0f, 256.0f, 0.0f), NewPos - btVector3(0.0f, 1.0f, 0.0f));
		if (!Responce) { return true; }

		SetPositionV(*Responce);

		return true;
	}
	bool Mixin_Movable::NeedsUpdate()
	{
		if((std::chrono::system_clock::now() - m_lastUpdate) > std::chrono::milliseconds(500))
		{
			return true;
		}
		if (m_velocity.length2() != 0)
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



		if (NewVel == m_velocity)
		{			
			if ((m_pos - (NewPos + (NewVel * GameTimeToSeconds(UpdateLag)))).length2() < (NewVel.length2() / 1000)) { return true; }
		}


		auto Responce = Mixin_Movable_Singleton::Instance().m_landPhys->RayTest(NewPos + btVector3(0.0f, 256.0f, 0.0f), NewPos - btVector3(0.0f, 1.0f, 0.0f));
		if (!Responce) { return true; }

		float MaxVelocity = 1.0f;
		if (NewVel.length2() > MaxVelocity*MaxVelocity)
		{//Velocity is bigger then the allowed maximum
			return false;
		}

		

		SetPositionV(*Responce);
		SetVelocityV(NewVel);

		return true;


	}

	void Mixin_Movable::ReciveEvent(const EventBase * const Event)
	{
		static int EventCount = 0;
		EventCount++;
		switch (Event->getEventID())
		{
		case KeyboardEvent_PlayerMoveBackward::id: {
			if (static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				LOGCATEGORY("EVENTS!").info("MoveBackwards.Pressed");
				m_movementState.Normal = CurrentMovement::NormalMovement::BACKWARD;
			}
			else
			{
				LOGCATEGORY("EVENTS!").info("MoveBackwards.Released");
				m_movementState.Normal = CurrentMovement::NormalMovement::NONE;
			}
			break;
		}
		case KeyboardEvent_PlayerMoveForward::id: {
			if (static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				LOGCATEGORY("EVENTS!").info("MoveForward.Preesed");
				m_movementState.Normal = CurrentMovement::NormalMovement::FORWARD;
			}
			else
			{
				LOGCATEGORY("EVENTS!").info("MoveForward.released");
				m_movementState.Normal = CurrentMovement::NormalMovement::NONE;
			}
			break;
		}

		case KeyboardEvent_PlayerMoveLeft::id: {
			if (static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				LOGCATEGORY("EVENTS!").info("MoveLeft.Preesed");
				m_movementState.Strafe = CurrentMovement::StrafeMovement::LEFT;
			}
			else
			{
				LOGCATEGORY("EVENTS!").info("MoveLeft.Released");
				m_movementState.Strafe = CurrentMovement::StrafeMovement::NONE;
			}
			break;
		}
		case KeyboardEvent_PlayerMoveRight::id: {
			if (static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				LOGCATEGORY("EVENTS!").info("MoveRight.Preesed");
				m_movementState.Strafe = CurrentMovement::StrafeMovement::RIGHT;
			}
			else
			{
				LOGCATEGORY("EVENTS!").info("MoveRight.Released");
				m_movementState.Strafe = CurrentMovement::StrafeMovement::NONE;
			}
			break;
		}

		default: {break; }
		}
		SetVelocityV(m_movementState.GetNewVelociyVectorBasedOnCurrentState()*m_speed);
		;
	}

	void Mixin_Movable::ApplyEvent(cereal::BinaryInputArchive& archive)
	{
		btScalar posx, posy, posz;
		archive(posx, posy, posz);
		SetPosition(posx, posy, posz);

		btScalar vex, vey, vez;
		archive(vex, vey, vez);

		if ((btVector3(vex,vey,vez).length() == 1.0f) && (m_velocity.length() == 0.0f))
		{
			printf("a");
		}

		SetVelocity(vex, vey, vez);
	}

	Mixin_Movable::CurrentMovement::CurrentMovement() : 
		Normal(NormalMovement::NONE), Strafe(StrafeMovement::NONE)
	{
	}

	btVector3 Mixin_Movable::CurrentMovement::GetNewVelociyVectorBasedOnCurrentState()
	{
		btVector3 NewVelocity = btVector3(0.0f, 0.0f, 0.0f);
		switch (Normal)
		{
		case NormalMovement::FORWARD: {NewVelocity += btVector3(0.0f, 0.0f, 1.0f); break; }
		case NormalMovement::BACKWARD: {NewVelocity -= btVector3(0.0f, 0.0f, 1.0f); break; }
		default: {break; }
		}
		switch (Strafe)
		{
		case StrafeMovement::RIGHT: {NewVelocity += btVector3(1.0f, 0.0f, 0.0f); break; }
		case StrafeMovement::LEFT: {NewVelocity -= btVector3(1.0f, 0.0f, 0.0f); break; }
		default: {break; }
		}

		if (NewVelocity.fuzzyZero()) { return btVector3(0.0f, 0.0f, 0.0f); }
		else
		{
			return	NewVelocity.normalized();
		}
	}

}
