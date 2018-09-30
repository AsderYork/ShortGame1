#include "GameSimulation.h"
#include "LogHelper.h"


namespace GEM::GameSim
{
	void GameSimulation::InsertCommand(ENTITY_ID_TYPE Entity, MixinCommandRetranslator && Command)
	{
		m_commandBuffer.push(std::make_pair(std::move(Command), Entity));
	}
	bool GameSimulation::Tick(float delta)
	{
		if (delta > 1.0f)
		{
			LOGCATEGORY("GameSimulation/Tick").warn("TimeDelta was %f which is too big! If this message repeats - server can't keep up!", delta);
			delta = 1.0;
		}
		auto GameTimeDelta = SecondsToGameTimeInterval(delta);
		//Apply all commands
		while (!m_commandBuffer.empty())
		{
			auto Entity = m_entities.GetEntity(m_commandBuffer.front().second).lock();
			if (Entity == nullptr) { continue; }
			Mixin_Controller::Instance().ApplyCommand(Entity.get(), std::move(m_commandBuffer.front().first));
			m_commandBuffer.pop();
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

		m_physics.Tick(GameTimeDelta);
		m_simulationTime += GameTimeDelta;
		return true;
	}


	std::weak_ptr<EntityBase> GameSimulation::AddEntity(const ENTITY_ID_TYPE ID, const std::vector<MIXIN_ID_TYPE> mixins)
	{
		auto ent = m_entities.AddFreeEntity(ID, m_generator.GenerateEntity(std::move(mixins), this, ID));
		return ent;
	}
	std::pair<std::weak_ptr<EntityBase>, ENTITY_ID_TYPE> GameSimulation::AddEntity(std::vector<MIXIN_ID_TYPE> mixins)
	{
		m_lastAddedEntity++;
		while (!m_entities.GetEntity(m_lastAddedEntity).expired())
		{
			if (m_lastAddedEntity == (std::numeric_limits<ENTITY_ID_TYPE>::max)())
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
