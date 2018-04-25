#pragma once

#include "EntitiesBase.h"
#include "GS_PlayerController.h"
#include "GS_EntityController.h"
#include "Mixin_Controller.h"
#include "EntityGenerator.h"
#include "EventBase.h"
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
		void RegisterMixins()
		{
			REGISTER_MIXIN_CLASS(Mixin_Movable);
				REGISTER_MIXIN_METHOD(Mixin_Movable, Shift, 1);
				REGISTER_MIXIN_METHOD(Mixin_Movable, SetVelocity, 2);

			REGISTER_MIXIN_CLASS(Mixin_Health);
			REGISTER_MIXIN_METHOD(Mixin_Health, SetHealth, 1);
		}

		std::queue<std::pair<MixinCommandRetranslator, ENTITY_ID_TYPE>> m_commandBuffer;


		/**!
		Used to generate entity ID when needed.
		*/
		ENTITY_ID_TYPE m_lastAddedEntity = 0;
	protected:
		GameTime m_simulationTime = 0;


	public:
		EntityController m_entities;
		EntityGenerator<Mixin_Health, Mixin_Movable> m_generator;
		PlayerController m_players;
		GamePhysics m_physics;


		std::queue<std::pair<std::unique_ptr<EventBase>, ENTITY_ID_TYPE>> m_eventsBuffer;
		
		inline const GameTime& getGameTime() { return m_simulationTime; }

		GameSimulation() {
			RegisterMixins();
			m_physics.Initialize();
		}

		/**!
		Inserts an event into simulation. Event will be applied to a all mixin of
		specified entity with the next call of \c Tick.
		*/
		void InsertEvent(std::unique_ptr<EventBase>&& Event, const ENTITY_ID_TYPE  id);



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
		std::pair<EntityBase*, ENTITY_ID_TYPE> AddEntity(std::vector<MIXIN_ID_TYPE> mixins);

		/**!
		Adds entity with a given ID.
		Returns a pointer to the entity, if it was created or nullptr, if entity wasn't created
		*/
		EntityBase* AddEntity(ENTITY_ID_TYPE ID, std::vector<MIXIN_ID_TYPE> mixins);



	};
}