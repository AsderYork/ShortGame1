#include "Mixin_Movable.h"
#include "KeyboardEvents.h"
#include <cereal\archives\binary.hpp>
#include "GameSimulation.h"
#include "NumerableEvents.h"

namespace GEM::GameSim
{
	void Mixin_Movable::SetVelocity(float X, float Y, float Z)
	{
		m_velocity = vmml::vec3f(X, Y, Z);
	}
	bool Mixin_Movable::tick(float delta)
	{
		m_pos += m_velocity;
		return true;
	}
	bool Mixin_Movable::NeedsUpdate()
	{
		if((std::chrono::system_clock::now() - m_lastUpdate) > std::chrono::milliseconds(500))
		{
			return true;
		}
		if (m_velocity.squared_length() != 0)
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

	void Mixin_Movable::ReciveUpdate(cereal::BinaryInputArchive & archive)
	{
		archive(m_pos.x(), m_pos.y(), m_pos.z());
		archive(m_velocity.x(), m_velocity.y(), m_velocity.z());
	}

	void Mixin_Movable::ReciveEvent(const EventBase * const Event)
	{
		switch (Event->getEventID())
		{
		case KeyboardEvent_PlayerMoveBackward::id: {
			if (!static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				m_velocity += vmml::vec3f(0.1f, 0.0f, 0.0f);
			}
			else
			{
				m_velocity -= vmml::vec3f(0.1f, 0.0f, 0.0f);
			}
			break;
		}
		case KeyboardEvent_PlayerMoveForward::id: {
			if (static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				m_velocity += vmml::vec3f(0.1f, 0.0f, 0.0f);
			}
			else
			{
				m_velocity -= vmml::vec3f(0.1f, 0.0f, 0.0f);
			}
			break;
		}

		case KeyboardEvent_PlayerMoveLeft::id: {
			if (!static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				m_velocity += vmml::vec3f(0.0f, 0.0f, 0.1f);
			}
			else
			{
				m_velocity -= vmml::vec3f(0.0f, 0.0f, 0.1f);
			}
			break;
		}
		case KeyboardEvent_PlayerMoveRight::id: {
			if (static_cast<const KeyboardEventBase*>(Event)->m_isPressed)
			{
				m_velocity += vmml::vec3f(0.0f, 0.0f, 0.1f);
			}
			else
			{
				m_velocity -= vmml::vec3f(0.0f, 0.0f, 0.1f);
			}
			break;
		}

		default: {break; }
		}
	}

}
