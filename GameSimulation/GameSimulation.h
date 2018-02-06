#pragma once

#include "EntitiesBase.h"
#include "GS_PlayerController.h"
#include "GS_EntityController.h"
#include "Mixin_Controller.h"

//Mixins
#include "Mixin_Movable.h"
#include "Mixin_Health.h"



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


	public:
		GameSim_PlayerController m_players;
		EntityController m_entities;

		GameSimulation() {
			RegisterMixins();
		}

		/**!
		Performs one tick of a simulation
		\returns true if everything is ok. If false is returned, simulation is broken and should be terminated
		*/
		bool Tick(float delta);



	};
}