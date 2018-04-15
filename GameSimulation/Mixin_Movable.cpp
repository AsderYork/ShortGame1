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

	void Mixin_Movable::SetVelocity(float X, float Y, float Z)
	{
		m_velocity = btVector3(X, Y, Z);
	}
	bool Mixin_Movable::tick(const GameTime delta)
	{
		m_pos += m_velocity * GameTimeToSeconds(delta);
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

	void Mixin_Movable::ReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime UpdateLag)
	{
		btVector3 NewPos;
		btScalar posx, posy, posz;
		archive(posx, posx, posx);
		NewPos.setValue(posx, posy, posz);

		btVector3 NewVel;
		btScalar vex, vey, vez;
		archive(vex, vey, vez);
		NewVel.setValue(vex, vey, vez);


		if (NewVel == m_velocity)
		{			
			if ((m_pos - (NewPos + (NewVel * GameTimeToSeconds(UpdateLag)))).length2() < (NewVel.length2() / 1000)) { return; }
		}

		m_pos = NewPos;
		m_velocity = NewVel;

	}

	void Mixin_Movable::ReciveEvent(const EventBase * const Event)
	{
		switch (Event->getEventID())
		{
		case KeyboardEvent_PlayerMoveBackward::id: {
			if (!static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				m_velocity += btVector3(m_speed, 0.0f, 0.0f);
			}
			else
			{
				m_velocity -= btVector3(m_speed, 0.0f, 0.0f);
			}
			break;
		}
		case KeyboardEvent_PlayerMoveForward::id: {
			if (static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				m_velocity += btVector3(m_speed, 0.0f, 0.0f);
			}
			else
			{
				m_velocity -= btVector3(m_speed, 0.0f, 0.0f);
			}
			break;
		}

		case KeyboardEvent_PlayerMoveLeft::id: {
			if (!static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				m_velocity += btVector3(0.0f, 0.0f, m_speed);
			}
			else
			{
				m_velocity -= btVector3(0.0f, 0.0f, m_speed);
			}
			break;
		}
		case KeyboardEvent_PlayerMoveRight::id: {
			if (static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				m_velocity += btVector3(0.0f, 0.0f, m_speed);
			}
			else
			{
				m_velocity -= btVector3(0.0f, 0.0f, m_speed);
			}
			break;
		}

		default: {break; }
		}
	}

	void Mixin_Movable::ApplyEvent(cereal::BinaryInputArchive& archive)
	{
		btScalar posx, posy, posz;
		archive(posx, posy, posz);
		m_pos.setValue(posx, posy, posz);

		btScalar vex, vey, vez;
		archive(vex, vey, vez);
		m_velocity.setValue(vex, vey, vez);
	}

}
