#include "EntityGenerator.h"

namespace GEM::GameSim
{
	Mixin_base * GEM::GameSim::StaticlyMixedEntity::GetMixinByID(int i)
	{
		for (auto& m : m_mixins)
		{
			if (m->getMixinID() == i) { return m.get(); }
		}
		return nullptr;
	}

	bool GEM::GameSim::StaticlyMixedEntity::tick(const GameTime delta)
	{
		for (auto& m : m_mixins)
		{
			if (!m->tick(delta)) { return false; }
		}
		return true;
	}

	std::vector<Mixin_base*> StaticlyMixedEntity::getAllMixins()
	{
		std::vector< Mixin_base*> ret;
		for (auto& m : m_mixins)
		{
			ret.emplace_back(m.get());
		}
		return ret;
	}

}