#pragma once
#include <SDL2\SDL.h>
#include <SDL2\SDL_syswm.h>
#include <vector>
#include "EngineController.h"

namespace GEM
{
	class SDL_Controller;
	/**
	\brief Lisents to a mouse events
	*/
	class SDL_MouseListener
	{
	protected:
		//Receives SDL_MOUSEMOTION and SDL_MOUSEWHEEL events
		virtual void mouseMoved(const SDL_Event &arg) = 0;
		virtual void mousePressed(const SDL_MouseButtonEvent &arg) = 0;
		virtual void mouseReleased(const SDL_MouseButtonEvent &arg) = 0;

		friend class SDL_Controller;
	};

	/**
	\brief Listens to a keyboard events. TextInput included
	*/
	class SDL_KeyboardListener
	{
	protected:
		virtual void textInput(const SDL_TextInputEvent& arg) = 0;
		virtual void keyPressed(const SDL_KeyboardEvent &arg) = 0;
		virtual void keyReleased(const SDL_KeyboardEvent &arg) = 0;

		friend class SDL_Controller;
	};


	/**!
	\brief This class allows controll of SDL. Creation of window, OS Messages processing and user input
	*/
	class SDL_Controller : public GEM::Service
	{
	public:
		virtual ActionResult initialize() override;

		virtual void shutdown() override;

		virtual ActionResult preFrame(float timeDelta) override;

		virtual ActionResult frame(float timeDelta) override;

		virtual ActionResult postFrame(float timeDelta) override;

		/**!
		Intializes SDL. For now it's the only thing, that happens in constructor.
		And yes, it's RAII.
		*/
		SDL_Controller();
		~SDL_Controller();

		/**!
		\brief Creates a window.
		\param[in] sizeX A width of window. In pixels
		\param[in] sizeY A height of window. In pixels
		\param[in] fullscreen Set to true, to make window occupy the entire screen, otherwise it'll be just a window. 
		\note SizeX and SizeY do set the resolution. Becouse how you'll show a 640x280 set of pixels in 10x10 window?
		\returns Returns HWND of the window as a string, or returns empty string, if window wasn't created.
		*/
		std::string MakeWindow(int sizeX=320, int sizeY=480, bool fullscreen=false);

		/**
		\brief registers a mouse listener
		This function does not transfer ownership, so the user must manage listener's time of live manually.
		User also must gurantee, that listener will survive untill at least of shutdown of SDL_Service.
		Listeners might be registered at any time, but not after SDL_Service's shutdown
		*/
		void registerMouseListener(SDL_MouseListener* listener);

		/**
		\brief registers a keyboard listener
		This function does not transfer ownership, so the user must manage listener's time of live manually.
		User also must gurantee, that listener will survive untill at least of shutdown of SDL_Service.
		Listeners might be registered at any time, but not after SDL_Service's shutdown
		*/
		void registerKeyboardListener(SDL_KeyboardListener* listener);

		bool processEvents();

	private:

		//The data, that is useful for window construction
		int m_residentScreen;//Screen on which window must appear. 0 for default one.
		SDL_Window *m_sdlWindow;
		SDL_SysWMinfo m_windowInfo;
		//Capture mouse, and prevent it from moving outside of the window, or let it go freely. 
		bool m_captureMouse = false;
		
		std::vector<SDL_MouseListener*> m_mouseVector;
		std::vector<SDL_KeyboardListener*> m_keyboardVector;


	};
}