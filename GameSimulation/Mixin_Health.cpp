#include "Mixin_Health.h"
#include <cereal\archives\binary.hpp>

namespace GEM::GameSim
{

	void Mixin_Health::SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason)
	{
		archive(m_health);
		archive(m_maxHealth);
	}

	bool Mixin_Health::CheckAndReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime updateTime)
	{
		return true;
	}

	void Mixin_Health::ApplyUpdate(cereal::BinaryInputArchive& archive)
	{
		archive(m_health);
		archive(m_maxHealth);
	}

	bool Mixin_Health::NeedsUpdate()
	{
		if ((std::chrono::system_clock::now() - m_lastUpdate) > std::chrono::milliseconds(500) || isChanged)
		{
			m_lastUpdate = std::chrono::system_clock::now();
			isChanged = false;
			return true;
		}
		

		return false;
	}
}
