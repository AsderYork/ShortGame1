#pragma once
#include <KeyboardEvents.h>
#include "SDL2_Service.h"
#include <string>
#include <queue>
#include <functional>






namespace GEM
{



	class PlayerInputEventGenerator
	{
	private:
		struct RegisteredEvent {
			GameSim::EVENT_ID_TYPE type;
			std::function<std::unique_ptr<GameSim::KeyboardEventBase>(bool)> createCallback;
			SDL_Scancode Scancode;
			SDL_Keymod Keymod;
			Uint8 MouseKeys;
			
			/**!
			So we hold too much info.
			This flags controlls, what exactly is needed to call this event.
			*/
			bool KeyMatters=true;
			bool MouseMetters=false;
			bool KeymodMetters=true;

			/**!
			When we check every event and see, that this particular event should be fired, we do that,
			but we don't want for it to be fired constantly, and we also want KeyReleased event. So we
			allow registered events to hold the state.
			*/
			bool isCalled = false;

			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(cereal::make_nvp("type", type));
				archive(cereal::make_nvp("Scancode", Scancode));
				archive(cereal::make_nvp("Keymod", Keymod));
				archive(cereal::make_nvp("KeyMatters", KeyMatters));
				archive(cereal::make_nvp("MouseMetters", MouseMetters));
				archive(cereal::make_nvp("KeymodMetters", KeymodMetters));
			}
			
		};

		std::vector<RegisteredEvent> m_events;
		
		std::queue<std::unique_ptr<GEM::GameSim::EventBase>> m_eventQueue;
		
	public:

		PlayerInputEventGenerator();


		std::unique_ptr<GEM::GameSim::EventBase> getEvent();
		

		template<class T>
		/**!
		Registers new type of event. Works only with KeyboardEvents, that have only it's 'pressed' state in a constructor
		*/
		void RegisterKeyboardEventType(SDL_Scancode defaultScancode, SDL_Keymod defaultKeymod = KMOD_NONE, Uint8 Mouse = 0,
			bool MouseMatters = false, bool KeyMatters = true,	bool KeymodMatters = false)
		{
			RegisteredEvent RegEvent;
			RegEvent.type = T::id;
			RegEvent.createCallback = [](bool isPressed) {return std::make_unique<T>(isPressed);};

			RegEvent.Keymod = defaultKeymod;
			RegEvent.Scancode = defaultScancode;
			RegEvent.MouseKeys = Mouse;

			RegEvent.KeyMatters = KeyMatters;
			RegEvent.KeymodMetters = KeymodMatters;
			RegEvent.MouseMetters = MouseMatters;

			m_events.emplace_back(RegEvent);
		}

		/**!
		Input configuration can be saved and load in a JSON form
		*/
		 bool LoadConfig(std::string config);
		 std::string SaveConfig();


		 void CheckForEvents();


	};
}