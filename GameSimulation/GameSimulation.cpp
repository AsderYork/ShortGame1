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


	EntityBase* GameSimulation::AddEntity(ENTITY_ID_TYPE ID, std::vector<MIXIN_ID_TYPE> mixins)
	{
		return m_entities.AddFreeEntity(ID, m_generator.GenerateEntity(std::move(mixins)));
	}
	std::pair<EntityBase*, ENTITY_ID_TYPE> GameSimulation::AddEntity(std::vector<MIXIN_ID_TYPE> mixins)
	{
		m_lastAddedEntity++;
		while (m_entities.GetEntity(m_lastAddedEntity) != nullptr)
		{
			if (m_lastAddedEntity == std::numeric_limits<ENTITY_ID_TYPE>::max())
			{
				m_lastAddedEntity = 0;
			}
			else
			{
				m_lastAddedEntity++;
			}
		}


		return std::make_pair(m_entities.AddFreeEntity(m_lastAddedEntity, m_generator.GenerateEntity(std::move(mixins))), m_lastAddedEntity);
	}

}
