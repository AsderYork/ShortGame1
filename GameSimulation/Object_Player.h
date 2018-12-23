#pragma once
#include <LogHelper.h>
#include "GameObject.h"
#include "Mixin_Movable.h"
#include "Mixin_Health.h"

#include "GameEvent_Base.h"


#include <limits>
#include <list>
#include <memory>
#include <assert.h>
#include <optional>

namespace GEM::GameSim
{
	/**
	Emits a deadly blast around the player, demaging every nearby players.
	*/
	class Object_Player_DeathBlast : public StaticGameEvent<Object_Player_DeathBlast>
	{
	public:
		virtual std::optional<bool> CheckValidity(const Dependecies & Deps) override;
		
		virtual void ApplyEvent(Dependecies & Deps) override;
		virtual void RollbackEvent(Dependecies& Deps) override;

	};

	class Object_Player_Heal : public StaticGameEvent<Object_Player_Heal>
	{
	public:
		virtual std::optional<bool> CheckValidity(const Dependecies & Deps) override;

		virtual void ApplyEvent(Dependecies & Deps) override;
		virtual void RollbackEvent(Dependecies& Deps) override;

	};


	class Object_Player : public GameObject<Mixin_Movable, Mixin_Health>
	{
	};

	


	
}