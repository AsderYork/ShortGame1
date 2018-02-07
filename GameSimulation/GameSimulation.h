#pragma once

#include "EntitiesBase.h"
#include "GS_PlayerController.h"
#include "GS_EntityController.h"
#include "Mixin_Controller.h"

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

			REGISTER_MIXIN_CLASS(Mixin_Health);
			REGISTER_MIXIN_METHOD(Mixin_Health, SetHealth, 1);
		}

		std::queue<std::pair<MixinCommandRetranslator, ENTITY_ID_TYPE>> m_commandBuffers[2];
		bool m_TickParity = false;

	public:
		GameSim_PlayerController m_players;
		EntityController m_entities;

		GameSimulation() {
			RegisterMixins();
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



	};
}