#include "EntityGenerator.h"

namespace GEM::GameSim
{
	Mixin_base * GEM::GameSim::StaticlyMixedEntity::GetMixinByID(int i)
	{
		for (auto& m : m_mixins)
		{
			if (m.first == i) { return m.second.get(); }
		}
		return nullptr;
	}

	bool GEM::GameSim::StaticlyMixedEntity::tick(float delta)
	{
		for (auto& m : m_mixins)
		{
			if (!m.second->tick(delta)) { return false; }
		}
		return true;
	}

}