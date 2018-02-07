#include "GameSimulation.h"


namespace GEM::GameSim
{
	void GameSimulation::InsertCommand(ENTITY_ID_TYPE Entity, MixinCommandRetranslator && Command)
	{
		m_commandBuffers[m_TickParity ? 0 : 1].push(std::make_pair(std::move(Command), Entity));
	}
	bool GameSimulation::Tick(float delta)
	{
		//Invert parity, to expose different buffer
		m_TickParity = !m_TickParity;
		auto& BufferInUse = m_commandBuffers[m_TickParity ? 1 : 0];

		//Apply all commands
		while (!BufferInUse.empty())
		{
			auto Entity = m_entities.GetEntity(BufferInUse.front().second);
			if (Entity == nullptr) { continue; }
			Mixin_Controller::Instance().ApplyCommand(Entity, std::move(BufferInUse.front().first));
		}
		
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
