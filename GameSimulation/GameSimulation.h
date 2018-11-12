#pragma once

#include "EntitiesBase.h"
#include "GS_PlayerController.h"
#include "GS_EntityController.h"
#include "Mixin_Controller.h"
#include "EntityGenerator.h"
#include "GameTime.h"
#include "GamePhysics.h"

//Mixins
#include "Mixin_Movable.h"
#include "Mixin_Health.h"

#include <queue>


namespace GEM::GameSim
{	

	class GameSimulation {
	private:
		std::queue<std::pair<MixinCommandRetranslator, ENTITY_ID_TYPE>> m_commandBuffer;


		/**!
		Used to generate entity ID when needed.
		*/
		ENTITY_ID_TYPE m_lastAddedEntity = 0;
	protected:
		GameTime m_simulationTime = 0;
		float m_simulationTimeScle = 1.0f;


	public:
		EntityController m_entities;
		EntityGenerator<Mixin_Health, Mixin_Movable> m_generator;
		PlayerController m_players;
		GamePhysics m_physics;

			
		inline const GameTime& getGameTime() { return m_simulationTime; }
		inline float getGameTimeScale() { return m_simulationTimeScle; }

		inline void setGameTime(GameTime NewTime) { m_simulationTime = NewTime; }
		inline void setGameTimeScale(float NewTimeScle) { m_simulationTimeScle = NewTimeScle; }

		inline GameSimulation()
		{
			m_physics.Initialize();
		}


		/**!
		Insert a command in a simulation, to be applied on the upcoming tick;
		\param[in] Entity ID of entity that should recive a command
		\param[in] Command A command, that should be applied to an entity

		\note At the moment of writing, behaviour is undefined, if ID command and id is ill-formed
		*/
		void InsertCommand(ENTITY_ID_TYPE Entity, MixinCommandRetranslator&& Command);

		/**!
		Performs one tick of a simulation
		\returns true if everything is ok. If false is returned, simulation is broken and should be terminated
		*/
		bool Tick(float delta);

		/**!
		Adds entity with some valid id.
		\returns a pair with first value - pointer to newly created entity and second - ID of that entity
		*/
		std::pair<std::weak_ptr<EntityBase>, ENTITY_ID_TYPE> AddEntity(std::vector<MIXIN_ID_TYPE> mixins);

		/**!
		Adds entity with a given ID.
		Returns a pointer to the entity, if it was created or nullptr, if entity wasn't created
		*/
		std::weak_ptr<EntityBase> AddEntity(ENTITY_ID_TYPE ID, std::vector<MIXIN_ID_TYPE> mixins);



	};
}