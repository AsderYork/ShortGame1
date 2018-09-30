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
		switch (arg.keysym.sym)
		{
		case SDLK_UP:
			m_moveForward = true;
			break;
		case SDLK_DOWN:
			m_moveBackwards = true;
			break;
		case SDLK_RIGHT:
			m_moveRight = true;
			break;
		case SDLK_LEFT:
			m_moveLeft = true;
			break;
		default:
			break;
		}

		if (arg.keysym.scancode == SDL_SCANCODE_GRAVE)
		{
			m_debugOverlay = true;
		}
	}

	void PlayerInputManager::InputReciver::keyReleased(const SDL_KeyboardEvent & arg)
	{
		switch (arg.keysym.sym)
		{
		case SDLK_UP:
			m_moveForward = false;
			break;
		case SDLK_DOWN:
			m_moveBackwards = false;
			break;
		case SDLK_RIGHT:
			m_moveRight = false;
			break;
		case SDLK_LEFT:
			m_moveLeft = false;
			break;
		default:
			break;
		}

		if (arg.keysym.scancode == SDL_SCANCODE_GRAVE)
		{
			m_debugOverlay = false;
		}
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
		//MixinMovablePtr->CombineRotation(btQuaternion(-m_listener.m_mouseShiftX*0.004f,0.0f, 0.0f));
		MixinMovablePtr->setRotation(btQuaternion(-m_listener.m_mouseShiftX*0.008f, 0.0f, 0.0f));

		float ForwardBackward = m_listener.m_moveForward - m_listener.m_moveBackwards;
		float LeftRight = m_listener.m_moveLeft - m_listener.m_moveRight;
		
		float speed = 1.0f;
		auto tmpVel = btVector3(static_cast<float>(LeftRight), 0.0f, static_cast<float>(ForwardBackward));
		if (!tmpVel.fuzzyZero())
		{
			tmpVel.normalize();
		}
		MixinMovablePtr->SetVelocity(tmpVel*speed);

		//MixinMovablePtr->setRotation(btQuaternion(btVector3(0.0f, 1.0f, 0.0f), m_listener._debugshift));

		m_listener.m_mouseShiftX = 0.0f;
		m_listener.m_mouseShiftY = 0.0f;
		m_listener.m_mouseShiftY = 0.0f;

		m_showDebugOverlay = m_listener.m_debugOverlay;
	}
}
