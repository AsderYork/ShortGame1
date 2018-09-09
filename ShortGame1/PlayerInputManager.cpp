#include "stdafx.h"
#include "PlayerInputManager.h"

#include <Mixin_Movable.h>
#include <LogHelper.h>

namespace GEM
{
	//-----------------------------------------------------
	//-----PLAYER_INPUT_MANAGER----------------------------
	//-----------------------------------------------------
	void PlayerInputManager::InputReciver::textInput(const SDL_TextInputEvent & arg)
	{
	}

	void PlayerInputManager::InputReciver::keyPressed(const SDL_KeyboardEvent & arg)
	{

	}

	void PlayerInputManager::InputReciver::keyReleased(const SDL_KeyboardEvent & arg)
	{
	}

	void PlayerInputManager::InputReciver::mouseMoved(const SDL_Event & arg)
	{
		m_mouseShiftX += arg.motion.xrel;
		m_mouseShiftY += arg.motion.yrel;
		m_mouseWheelShift += arg.wheel.y;
	}

	void PlayerInputManager::InputReciver::mousePressed(const SDL_MouseButtonEvent & arg)
	{
	}

	void PlayerInputManager::InputReciver::mouseReleased(const SDL_MouseButtonEvent & arg)
	{
	}
	void PlayerInputManager::Apply(float TimeDelta)
	{
		auto LockedPlayerEntity = m_playerEnt.lock();
		if (!LockedPlayerEntity)
		{
			LOGCATEGORY("PlayerInputManager/Apply").error("Can't apply input changes. PlayerEntity is unavaliable!");
			throw std::exception("Player entity is unavaliable");
		}
		auto MixinMovablePtr = static_cast<GameSim::Mixin_Movable*>(LockedPlayerEntity->GetMixinByID(GameSim::Mixin_Movable::MixinID));

		//MixinMovablePtr->CombineRotation(btQuaternion(-m_listener.m_mouseShiftX*0.004f, m_listener.m_mouseShiftY*0.01f, 0.0f));
		MixinMovablePtr->CombineRotation(btQuaternion(-m_listener.m_mouseShiftX*0.004f,0.0f, 0.0f));



		m_listener.m_mouseShiftX = 0.0f;
		m_listener.m_mouseShiftY = 0.0f;
		m_listener.m_mouseShiftY = 0.0f;
	}
}
