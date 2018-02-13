#include "GS_Server.h"
#include <cereal\archives\binary.hpp>
#include <sstream>

bool GEM::GameSim::GS_Server::Tick(float Delta)
{
	auto Reval = m_gs.Tick(Delta);
	//Gather all the changes.
	std::unique_ptr<EntityController::EntityListIterator> iter = nullptr;

	auto Entity = m_gs.m_entities.IterateOverEntities(std::move(iter));
	while (Entity)
	{
		std::stringstream entityData;
		cereal::BinaryOutputArchive ar(entityData);
		ar(Entity->first);

		auto& mixinvec = Entity->second->getAllMixins();
		for (auto& mixin : mixinvec)
		{
			if (mixin->NeedsUpdate())
			{
				
				//mixin->SendUpdate
			}
		}


		auto Entity = m_gs.m_entities.IterateOverEntities(std::move(iter));
	}
	return Reval;
}
