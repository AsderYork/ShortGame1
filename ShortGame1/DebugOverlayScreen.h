#pragma once
#include "ScreenController.h"
#include "CEGUI_Service.h"
#include "SDL2_Service.h"

namespace GEM
{
	/**
	\brief A screen overlay with various debug info.

	This screen provides a semi-transparant overlay with info such as FPS, cardinal directions, current position and
	various states.
	*/
	class DebugOverlayScreen : public Screen
	{
	private:
		CEGUI::Window* m_mainWindow;
		CEGUI::Window* m_text;

		float m_timePassed = 0.0f;
		int m_framesPassed = 0;

		class InputListener : public SDL_KeyboardListener
		{
		public:
			// Унаследовано через SDL_KeyboardListener
			virtual void textInput(const SDL_TextInputEvent & arg) override;
			virtual void keyPressed(const SDL_KeyboardEvent & arg) override;
			virtual void keyReleased(const SDL_KeyboardEvent & arg) override;


			bool m_shouldClose = false;
		};

		InputListener m_listener;

	public:

		inline DebugOverlayScreen(SDL_Controller* service) : Screen(true)
		{
			service->registerKeyboardListener(&m_listener);
		}

		virtual bool Init() override;
		virtual void OnTop() override;
		virtual void NoLongerOnTop() override;
		virtual void LeaveStack() override;
		virtual void PostFrame(float Delta) override;
	};
}