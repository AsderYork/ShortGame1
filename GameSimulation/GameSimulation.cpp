#include "GameSimulation.h"


namespace GEM::GameSim
{
	bool GameSimulation::Tick(float delta)
	{
		std::unique_ptr<EntityController::EntityListIterator> iter = nullptr;

		auto Entity = m_entities.IterateOverEntities(std::move(iter));
		while (Entity)
		{
			if (!Entity->second->tick(delta))
			{
				return false;
			}
			Entity = m_entities.IterateOverEntities(std::move(iter));
		}
		return true;
	}
}
