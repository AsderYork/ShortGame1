#pragma once
#include <SDL2\SDL.h>
#include <SDL2\SDL_syswm.h>
#include "EngineController.h"

namespace GEM
{
	/**!
	This class allows controll of SDL. Creation of window, OS Messages processing and user input
	*/
	class SDL_Controller : public GEM::Service
	{
	public:
		virtual ActionResult initialize() override;

		virtual void shutdown() override;

		virtual ActionResult preFrame(double timeDelta) override;

		virtual ActionResult frame(double timeDelta) override;

		virtual ActionResult postFrame(double timeDelta) override;

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
		bool processEvents();

	private:
		//The data, that is useful for window construction
		int m_residentScreen;//Screen on which window must appear. 0 for default one.
		SDL_Window *m_sdlWindow;
		SDL_SysWMinfo m_windowInfo;
		


	};
}