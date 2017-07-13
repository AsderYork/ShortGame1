#pragma once
#include <SDL2\SDL.h>
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

		bool MakeWindow();
		bool processEvents();

	private:
		//The data, that is useful for window construction
		int m_residentScreen;//Screen on which window must appear. 0 for default one.
		SDL_Window *m_sdlWindow;
		


	};
}