#include "EntityGenerator.h"

namespace GEM::GameSim
{
	Mixin_base * GEM::GameSim::StaticlyMixedEntity::GetMixinByID(int i) const
	{
		for (auto& m : m_mixins)
		{
			if (m->getMixinID() == i) { return m.get(); }
		}
		return nullptr;
	}

	bool GEM::GameSim::StaticlyMixedEntity::tick(float delta)
	{
		for (auto& m : m_mixins)
		{
			if (!m->tick(delta)) { return false; }
		}
		return true;
	}

	std::vector<Mixin_base*> const StaticlyMixedEntity::getAllMixins() const
	{
		std::vector<Mixin_base*> ret;
		for (auto& m : m_mixins)
		{
			ret.emplace_back(m.get());
		}
		return ret;
	}

}