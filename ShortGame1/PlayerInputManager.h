#pragma once
#include "SDL2_Service.h"
#include "EventsController.h"
#include "InputEvent.h"

#include <LinearMath/btQuaternion.h>
#include <EntitiesBase.h>
#include <memory>
#include <vector>

namespace GEM
{
	/**!
	Translates playerInput into changes of a game-state

	There is alrready a system, that basically does the same. It's PlayerInputEventGenerator
	and the whole Events Systems. But it's extremely inconvinent to use. For every change you want to apply to an entity,
	you must register an event, describe, what this event means to a PIEG and provide a reaction description to a Mixin.
	It gets even messier when you consider the fact, that the Effect of the player input does not depend on the type of mixins
	in an entity, but instead it depends on the overall state of the game, and of the GUI!
	So That system just wasn't up to the task, and that's why this sistem is being created.


	*/
	class PlayerInputManager
	{
	private:


		bool m_showDebugOverlay = false;
		bool m_isPropagating = true;
		std::weak_ptr<GameSim::EntityBase> m_playerEnt;

		std::vector<InputEvent_Button> m_lastButtonPresses;
		InputEvent_MouseState m_lastMouseState;

		/**!
		PlayerInput Listener.
		This interface is separated from PIM, to hide it's interface
		*/
		class InputReciver : public SDL_KeyboardListener, SDL_MouseListener
		{
		public:
			bool m_moveForward = false;
			bool m_moveBackwards = false;
			bool m_moveLeft = false;
			bool m_moveRight = false;

			bool m_debugOverlay = false;
			bool m_attack = false;

			std::vector<InputEvent_Button> m_lastButtonPresses;
			InputEvent_MouseState m_lastMouseState;


			float m_mouseShiftX = 0.0f;
			float m_mouseShiftY = 0.0f;
			float m_mouseWheelShift = 0.0f;
			// Унаследовано через SDL_KeyboardListener
			virtual void textInput(const SDL_TextInputEvent & arg) override;
			virtual void keyPressed(const SDL_KeyboardEvent & arg) override;
			virtual void keyReleased(const SDL_KeyboardEvent & arg) override;

			// Унаследовано через SDL_MouseListener
			virtual void mouseMoved(const SDL_Event & arg) override;
			virtual void mousePressed(const SDL_MouseButtonEvent & arg) override;
			virtual void mouseReleased(const SDL_MouseButtonEvent & arg) override;

			friend class PlayerInputManager;
		};
		friend class InputReciver;

		InputReciver m_listener;

		GameEventsController* m_events;

	public:



		PlayerInputManager(GameEventsController* eventsController);

		int m_playerID;
		/**
		\brief Shows weather MainGameScreenShould call a DeubugOvrlay over itself
		*/
		inline bool ShouldShowDebugOverlay()
		{
			return m_showDebugOverlay;
		}

		/**
		\brief Show wether actual player entity is acuired by the Manager
		*/
		inline bool isPlayerEntityAccuried()
		{
			return !m_playerEnt.expired();
		}

		/**
		\returns Propagation state

		If PIM is propagating, then it is sending recived input to a GameSimulation.
		Otherwise PIM will not affect GameSim.
		*/
		inline bool const getPropagationState() { return m_isPropagating; }

		/**
		Sets the propagation state
		If PIM is propagating, then it is sending recived input to a GameSimulation.
		Otherwise PIM will not affect GameSim.
		*/
		inline void setProagationState(bool NewState) { m_isPropagating = NewState; }
		inline bool getProagationState(){ return m_isPropagating; }

		/**
		Registers internalController to the SDLService
		This method must be called before user input is actually needed
		*/
		inline void RegisterListener(SDL_Controller* SDLController) {
			SDLController->registerKeyboardListener(&m_listener);
			SDLController->registerMouseListener(&m_listener);
		}

		/**
		Provides PlayerEntity to a PIM

		PIM can't controll an entity, if it doesn't now, what entity to controll
		This method must be called to provide access to a playerEntity to a PIM.

		\note It is possible, to provide other Entitiy then player's to controll it in simmilar fasion,
		but provided entity MUST provide at least the same interface, as Player's
		*/
		inline void AccuirePlayerEntity(std::weak_ptr<GameSim::EntityBase> Ent) { m_playerEnt = Ent; }

		/**
		Applies input actions to the simulation

		InputSystem is connected to the input controller throgh the callbacks which that changes can occur in any moment
		during the working time, which is not very good. To counter that, this manager stores changes, that are happening
		throughout the game loop and applies them only with the call to this method, if the propagation is turned on.
		\note input controller actually can call callbacks only durning it's time on the main thread, but it's behaviour
		might be changed later, so it's not a good idea to rely on that

		\param[in] TimeDelta A time passed since the last call
		*/
		void Apply(float TimeDelta, btQuaternion CameraOrient);

		const InputEvent_MouseState& MouseState() { return m_lastMouseState; }
		const std::vector<InputEvent_Button>& ButtonHistory() { return m_lastButtonPresses; }
	};
}
