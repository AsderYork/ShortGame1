#include "GameSimulation.h"


namespace GEM::GameSim
{
	void GameSimulation::InsertEvent(std::unique_ptr<EventBase>&& Event, const ENTITY_ID_TYPE id)
	{
		m_eventsBuffer.emplace(std::move(Event), id);
	}
	void GameSimulation::InsertCommand(ENTITY_ID_TYPE Entity, MixinCommandRetranslator && Command)
	{
		m_commandBuffer.push(std::make_pair(std::move(Command), Entity));
	}
	bool GameSimulation::Tick(float delta)
	{
		auto GameTimeDelta = SecondsToGameTimeInterval(delta);
		//Apply all commands
		while (!m_commandBuffer.empty())
		{
			auto Entity = m_entities.GetEntity(m_commandBuffer.front().second);
			if (Entity == nullptr) { continue; }
			Mixin_Controller::Instance().ApplyCommand(Entity, std::move(m_commandBuffer.front().first));
			m_commandBuffer.pop();
		}

		//Apply all events. Commands is deprecated now, don't they?
		while (!m_eventsBuffer.empty())
		{
			for (auto& mixin : m_entities.GetEntity(m_eventsBuffer.front().second)->getAllMixins())
			{
				mixin->ReciveEvent(m_eventsBuffer.front().first.get());
			}
			m_eventsBuffer.pop();
		}
		
		std::unique_ptr<EntityController::EntityListIterator> iter = nullptr;

		auto Entity = m_entities.IterateOverEntities(std::move(iter));
		while (Entity)
		{
			if (!Entity->second->tick(GameTimeDelta))
			{
				return false;
			}
			Entity = m_entities.IterateOverEntities(std::move(iter));
		}

		m_simulationTime += GameTimeDelta;
		return true;
	}


	EntityBase* GameSimulation::AddEntity(const ENTITY_ID_TYPE ID, const std::vector<MIXIN_ID_TYPE> mixins)
	{
		auto ent = m_entities.AddFreeEntity(ID, m_generator.GenerateEntity(std::move(mixins), this, ID));
		return ent;
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
		return std::make_pair(AddEntity(m_lastAddedEntity, std::move(mixins)), m_lastAddedEntity);
	}

}
