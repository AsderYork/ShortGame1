#include "GameSimulation.h"


namespace GEM::GameSim
{
	void GameSimulation::InsertCommand(ENTITY_ID_TYPE Entity, MixinCommandRetranslator && Command)
	{
		m_commandBuffer.push(std::make_pair(std::move(Command), Entity));
	}
	bool GameSimulation::Tick(float delta)
	{

		//Apply all commands
		while (!m_commandBuffer.empty())
		{
			auto Entity = m_entities.GetEntity(m_commandBuffer.front().second);
			if (Entity == nullptr) { continue; }
			Mixin_Controller::Instance().ApplyCommand(Entity, std::move(m_commandBuffer.front().first));
			m_commandBuffer.pop();
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
