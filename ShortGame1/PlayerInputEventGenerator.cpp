#include "stdafx.h"
#include "PlayerInputEventGenerator.h"
#include <cereal\cereal.hpp>
#include <cereal\archives\xml.hpp>
#include <cereal\types\vector.hpp>
#include <sstream>

namespace GEM
{
	
	PlayerInputEventGenerator::PlayerInputEventGenerator()
	{
		RegisterKeyboardEventType<GameSim::KeyboardEvent_PlayerMoveForward>(SDL_SCANCODE_UP);
		RegisterKeyboardEventType<GameSim::KeyboardEvent_PlayerMoveBackward>(SDL_SCANCODE_DOWN);
		RegisterKeyboardEventType<GameSim::KeyboardEvent_PlayerMoveLeft>(SDL_SCANCODE_LEFT);
		RegisterKeyboardEventType<GameSim::KeyboardEvent_PlayerMoveRight>(SDL_SCANCODE_RIGHT);
		RegisterKeyboardEventType<GameSim::KeyboardEvent_PlayerJump>(SDL_SCANCODE_SPACE);
		RegisterKeyboardEventType<GameSim::KeyboardEvent_PlayerAttack>(SDL_SCANCODE_UNKNOWN, KMOD_NONE, SDL_BUTTON_LEFT, true, false, false);
	}
	std::unique_ptr<GameSim::EventBase> PlayerInputEventGenerator::getEvent()
	{
		if (m_eventQueue.empty()) { return nullptr; }
		auto tmp = std::move(m_eventQueue.front());
		m_eventQueue.pop();
		return tmp;
	}
	bool PlayerInputEventGenerator::LoadConfig(std::string config)
	{
		std::vector<RegisteredEvent> loadedEvents;
		std::stringstream str;
		str.str(config);

		try {
			cereal::XMLInputArchive ar(str);
			ar(loadedEvents);
		}
		catch (...) { return false; }

		for (auto& ev : loadedEvents)
		{
			for (auto& inside_ev : m_events)
			{
				if (ev.type == inside_ev.type) { ev.createCallback = inside_ev.createCallback; break; }
			}
			if (!ev.createCallback) { return false; }
		}
		m_events.swap(loadedEvents);
		return true;
	}
	std::string PlayerInputEventGenerator::SaveConfig()
	{
		std::stringstream Output;
		{
			cereal::XMLOutputArchive ar(Output);
			ar(m_events);
		}		
		return Output.str();
	}
	void PlayerInputEventGenerator::CheckForEvents()
	{
		auto Keyboard = SDL_GetKeyboardState(nullptr);
		auto Modes = SDL_GetModState();
		auto MouseKeys = SDL_GetMouseState(nullptr, nullptr);

		for (auto& ev : m_events)
		{
			bool isPressed = false;
			int Decision = 0;
			if (ev.MouseMetters) {
				Decision += (ev.MouseKeys & MouseKeys); }
			if (ev.KeyMatters) { Decision += Keyboard[ev.Scancode]; }
			if (ev.KeymodMetters) { Decision += (ev.Keymod == Modes); }

			if (Decision == (ev.MouseMetters + ev.KeyMatters + ev.KeymodMetters))
			{
				isPressed = true;
			}

			if (isPressed != ev.isCalled)
			{
				m_eventQueue.emplace(ev.createCallback(isPressed));
				ev.isCalled = isPressed;
			}
		}
	}

}
