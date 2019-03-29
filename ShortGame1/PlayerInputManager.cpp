#include "stdafx.h"
#include "PlayerInputManager.h"

#include <Object_Player.h>

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
		m_lastButtonPresses.emplace_back(arg.keysym.scancode, true);
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
		case SDLK_SPACE:
			m_attack = true;
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
		m_lastButtonPresses.emplace_back(arg.keysym.scancode, true);
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
		case SDLK_SPACE:
			m_attack = false;
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
		m_lastMouseState.curr_x = arg.motion.x;
		m_lastMouseState.curr_y = arg.motion.y;
		m_lastMouseState.rel_x += arg.motion.xrel;
		m_lastMouseState.rel_y += arg.motion.yrel;

		if (arg.type == SDL_MOUSEWHEEL) {
			m_lastMouseState.rel_wheel += arg.wheel.y;
			m_mouseWheelShift += arg.wheel.y;
		}

		m_mouseShiftX += arg.motion.xrel;
		m_mouseShiftY += arg.motion.yrel;
	}

	void PlayerInputManager::InputReciver::mousePressed(const SDL_MouseButtonEvent & arg)
	{
		switch (arg.button)
		{
		case SDL_BUTTON_LEFT:	{m_lastMouseState.leftButton = true; break;}
		case SDL_BUTTON_RIGHT:	{m_lastMouseState.rightButton = true; break;}
		case SDL_BUTTON_MIDDLE: {m_lastMouseState.middleButton = true; break;}
		default:
			break;
		}
	}

	void PlayerInputManager::InputReciver::mouseReleased(const SDL_MouseButtonEvent & arg)
	{
		switch (arg.button)
		{
		case SDL_BUTTON_LEFT: {m_lastMouseState.leftButton = false; break; }
		case SDL_BUTTON_RIGHT: {m_lastMouseState.rightButton = false; break; }
		case SDL_BUTTON_MIDDLE: {m_lastMouseState.middleButton = false; break; }
		default:
			break;
		}
	}

	PlayerInputManager::PlayerInputManager(GameEventsController * eventsController) : m_events(eventsController)
	{
	}

	void PlayerInputManager::Apply(float TimeDelta, btQuaternion CameraOrient)
	{
		m_lastMouseState.curr_x = m_listener.m_lastMouseState.curr_x;
		m_lastMouseState.curr_y = m_listener.m_lastMouseState.curr_y;
		
		m_lastMouseState.leftButton = m_listener.m_lastMouseState.leftButton;
		m_lastMouseState.rightButton = m_listener.m_lastMouseState.rightButton;
		m_lastMouseState.middleButton = m_listener.m_lastMouseState.middleButton;

		m_lastMouseState.rel_x = m_listener.m_lastMouseState.rel_x;
		m_listener.m_lastMouseState.rel_x = 0;

		m_lastMouseState.rel_y = m_listener.m_lastMouseState.rel_y;
		m_listener.m_lastMouseState.rel_y = 0;

		m_lastMouseState.rel_wheel = m_listener.m_lastMouseState.rel_wheel;
		m_listener.m_lastMouseState.rel_wheel = 0;

		m_lastButtonPresses.clear();
		m_lastButtonPresses.swap(m_listener.m_lastButtonPresses);


		auto LockedPlayerEntity = m_playerEnt.lock();
		if (!LockedPlayerEntity)
		{
			LOGCATEGORY("PlayerInputManager/Apply").error("Can't apply input changes. PlayerEntity is unavaliable!");
			throw std::exception("Player entity is unavaliable");
		}
		auto MixinMovablePtr = static_cast<GameSim::Mixin_Movable*>(LockedPlayerEntity->GetMixinByID(GameSim::Mixin_Movable::MixinID));

		//MixinMovablePtr->CombineRotation(btQuaternion(-m_listener.m_mouseShiftX*0.004f, m_listener.m_mouseShiftY*0.01f, 0.0f));
		//MixinMovablePtr->CombineRotation(btQuaternion(-m_listener.m_mouseShiftX*0.004f,0.0f, 0.0f));
		//MixinMovablePtr->setRotation(btQuaternion(-m_listener.m_mouseShiftX*0.008f, 0.0f, 0.0f));

		float ForwardBackward = m_listener.m_moveForward - m_listener.m_moveBackwards;
		float LeftRight = m_listener.m_moveLeft - m_listener.m_moveRight;
		
		float speed = 1.0f;
		auto tmpVel = btVector3(static_cast<float>(LeftRight), 0.0f, static_cast<float>(ForwardBackward));

		if (m_lastMouseState.rightButton) {
			float theta = atan2(CameraOrient.y(), CameraOrient.w());

			// quaternion representing rotation about the y axis
			MixinMovablePtr->SetOrientation(btQuaternion(0, sin(theta), 0, cos(theta)));
		}

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

		static float Cooldown = 0;
		if (Cooldown > 0) { Cooldown -= TimeDelta; }
		if (Cooldown < 0) { Cooldown = 0; }

		if (m_listener.m_attack && Cooldown == 0)
		{
			Cooldown += 0.5;
		
			m_events->AddEvent(GameSim::Object_Player_DeathBlast::staticid(), LockedPlayerEntity->m_id);
			//m_eventsController->AddEvent<GameSim::Object_Player_DeathBlast>(LockedPlayerEntity->m_id);
		}
	}
}
