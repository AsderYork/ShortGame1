#include "stdafx.h"
#include "SDL2_Service.h"
#include <cassert>

namespace GEM
{
	SDL_Controller::SDL_Controller() : m_sdlWindow(nullptr)
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			LOGCATEGORY("SDL_Cntroller/Constructor").crit("%s", SDL_GetError());
			/*If this is happening, then SDL_Controller should be counted as broken. But it has no way of telling that.
			Anyway, writes a log-message. So if it appered and something is not working, then broken initialization is the first place to check.
			*/
		}
	}
	SDL_Controller::~SDL_Controller()
	{
		if (m_sdlWindow)
		{
			// Restore desktop resolution on exit
			SDL_SetWindowFullscreen(m_sdlWindow, 0);
			SDL_DestroyWindow(m_sdlWindow);
			m_sdlWindow = nullptr;
		}
		SDL_Quit();
	}
	std::string SDL_Controller::MakeWindow(int sizeX, int sizeY, bool fullscreen)
	{
		assert(m_sdlWindow == nullptr);//There can't be more then one window!


		int posX = SDL_WINDOWPOS_CENTERED_DISPLAY(1);
		int posY = SDL_WINDOWPOS_CENTERED_DISPLAY(1);

		m_sdlWindow = SDL_CreateWindow("A window",
			posX,               // initial x position
			posY,               // initial y position
			sizeX,              // width, in pixels
			sizeY,             // height, in pixels
			SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));

		SDL_VERSION(&m_windowInfo.version);
		if (SDL_GetWindowWMInfo(m_sdlWindow, &m_windowInfo) == SDL_FALSE)
		{
			LOGCATEGORY("SDL_Controller/MakeWindow").crit("Can't get info about window. %s", SDL_GetError());
			return std::string();
		}
		
		return std::to_string((uintptr_t)m_windowInfo.info.win.window);
	}
	bool SDL_Controller::processEvents()
	{
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_WINDOWEVENT:
				break;
			case SDL_QUIT:
				return false;
				break;
			default:
				break;
			}
		}
		return true;
	}
	
	GEM::Service::ActionResult SDL_Controller::initialize()
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			LOGCATEGORY("SDL_Cntroller/Constructor").crit("%s", SDL_GetError());
			return ActionResult::AR_ERROR;
		}
		MakeWindow();
		return  ActionResult::AR_OK;
	}
	void SDL_Controller::shutdown()
	{
		if (m_sdlWindow)
		{
			// Restore desktop resolution on exit
			SDL_SetWindowFullscreen(m_sdlWindow, 0);
			SDL_DestroyWindow(m_sdlWindow);
			m_sdlWindow = nullptr;
		}
		SDL_Quit();
	}
	GEM::Service::ActionResult SDL_Controller::preFrame(double timeDelta)
	{
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_WINDOWEVENT:
				break;
			case SDL_QUIT:
				LOGCATEGORY("SDL_Controller/preFrame").info("Quit event is cought! Ah, that was a beautiful time!");
				getEngineController()->shutdown();
				break;
			default:
				break;
			}
		}
		return ActionResult::AR_OK;
	}
	GEM::Service::ActionResult SDL_Controller::frame(double timeDelta)
	{
		return ActionResult();
	}
	GEM::Service::ActionResult SDL_Controller::postFrame(double timeDelta)
	{
		return ActionResult();
	}
}
