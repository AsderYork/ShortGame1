#include "stdafx.h"
#include "ScreenController.h"


namespace GEM
{
	//SCREENCONTROLLER
	void ScreenController::CleanListUp()
	{
		std::vector<std::list<std::unique_ptr<Screen>>::iterator> markedForDelition;
		for (auto it = m_screens.begin(); it != m_screens.end(); it++)
		{
			if (it->get()->m_shouldBeDeleated) { markedForDelition.push_back(it); }
		}

		for (auto& it : markedForDelition) { m_screens.erase(it); }
	}
	ScreenController::ScreenController(SDL_Controller * SDL)
	{
		SDL->registerKeyboardListener(this);
		SDL->registerMouseListener(this);
	}

	void ScreenController::textInput(const SDL_TextInputEvent & arg)
	{
		CleanListUp();
		for (auto& screen : m_screens)
		{
			screen->textInput(arg);
		}
	}
	void ScreenController::keyPressed(const SDL_KeyboardEvent & arg)
	{
		CleanListUp();
		for (auto& screen : m_screens)
		{
			screen->keyPressed(arg);
		}
	}
	void ScreenController::keyReleased(const SDL_KeyboardEvent & arg)
	{
		CleanListUp();
		for (auto& screen : m_screens)
		{
			screen->keyReleased(arg);
		}
	}
	void ScreenController::mouseMoved(const SDL_Event & arg)
	{
		CleanListUp();
		for (auto& screen : m_screens)
		{
			screen->mouseMoved(arg);
		}
	}
	void ScreenController::mousePressed(const SDL_MouseButtonEvent & arg)
	{
		CleanListUp();
		for (auto& screen : m_screens)
		{
			screen->mousePressed(arg);
		}
	}
	void ScreenController::mouseReleased(const SDL_MouseButtonEvent & arg)
	{
		CleanListUp();
		for (auto& screen : m_screens)
		{
			screen->mouseReleased(arg);
		}
	}

	Service::ActionResult ScreenController::initialize()
	{
		m_initialized = true;
		for (auto& screen : m_screens)
		{
			screen->Init();
		}
		return ActionResult::AR_OK;
	}
	void ScreenController::shutdown()
	{
		m_screens.clear();
	}
	Service::ActionResult ScreenController::preFrame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}
	Service::ActionResult ScreenController::frame(float timeDelta)
	{
		return ActionResult::AR_OK;
	}
	Service::ActionResult ScreenController::postFrame(float timeDelta)
	{
		CleanListUp();
		for (auto& screen : m_screens)
		{
			screen->PostFrame(timeDelta);
		}
		return ActionResult::AR_OK;
	}




	//SCREEN
	void Screen::Finish()
	{
		m_shouldBeDeleated = true;
	}
}