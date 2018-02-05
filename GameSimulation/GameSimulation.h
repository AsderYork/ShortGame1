#pragma once

#include "EntitiesBase.h"
#include "GS_PlayerController.h"
#include "GS_EntityController.h"



namespace GEM::GameSim
{	
	class GameSimulation {
		public:
		GameSim_PlayerController m_players;
		EntityController m_entities;

	};
}