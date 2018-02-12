#include "Mixin_Movable.h"
#include <cereal\archives\binary.hpp>

namespace GEM::GameSim
{
	bool Mixin_Movable::tick(float delta)
	{
		m_pos += m_velocity;
		return true;
	}
	bool Mixin_Movable::NeedsUpdate()
	{
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
		archive(m_pos.x(), m_pos.y(), m_pos.z());
		archive(m_velocity.x(), m_velocity.y(), m_velocity.z());
	}

	void Mixin_Movable::ReciveUpdate(cereal::BinaryOutputArchive & archive)
	{
		archive(m_pos.x(), m_pos.y(), m_pos.z());
		archive(m_velocity.x(), m_velocity.y(), m_velocity.z());
	}

}
