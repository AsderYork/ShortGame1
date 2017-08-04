#include "stdafx.h"
#include "CEGUI_Service.h"
#include <CEGUI\CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

namespace GEM
{
	CEGUI::Key::Scan toCEGUIKey(SDL_Scancode key)
	{
		switch (key)
		{
		case SDL_SCANCODE_ESCAPE: return CEGUI::Key::Scan::Esc;
		case SDL_SCANCODE_F1: return CEGUI::Key::Scan::F1;
		case SDL_SCANCODE_F2: return CEGUI::Key::Scan::F2;
		case SDL_SCANCODE_F3: return CEGUI::Key::Scan::F3;
		case SDL_SCANCODE_F4: return CEGUI::Key::Scan::F4;
		case SDL_SCANCODE_F5: return CEGUI::Key::Scan::F5;
		case SDL_SCANCODE_F6: return CEGUI::Key::Scan::F6;
		case SDL_SCANCODE_F7: return CEGUI::Key::Scan::F7;
		case SDL_SCANCODE_F8: return CEGUI::Key::Scan::F8;
		case SDL_SCANCODE_F9: return CEGUI::Key::Scan::F9;
		default: return CEGUI::Key::Scan::Unknown;
		}
	}

	CEGUI::MouseButton SDLtoCEGUIMouseButton(const Uint8& button)
	{
		using namespace CEGUI;

		switch (button)
		{
		case SDL_BUTTON_LEFT:
			return CEGUI::MouseButton::Left;

		case SDL_BUTTON_MIDDLE:
			return CEGUI::MouseButton::Middle;

		case SDL_BUTTON_RIGHT:
			return CEGUI::MouseButton::Right;

		default:
			return CEGUI::MouseButton::Invalid;
		}
	}




	CEGUI_Service::CEGUI_Service(Ogre_Service * ogreService, SDL_Controller * sdlController) :
		m_ogreService(ogreService),
		m_sdlController(sdlController)
	{

	}
	Service::ActionResult CEGUI_Service::initialize()
	{
		try
		{
			CEGUI::OgreRenderer* renderer = &CEGUI::OgreRenderer::bootstrapSystem( *(m_ogreService->getRoot()->getRenderTarget("A window")) );
			CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
			auto myRoot = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "_MasterRoot");
			CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(myRoot);
			CEGUI::System::getSingleton().getDefaultGUIContext().getCursor().setDefaultImage("TaharezLook/MouseArrow");

			m_inputAgregator = std::make_unique<CEGUI::InputAggregator>(&(CEGUI::System::getSingleton().getDefaultGUIContext()));
			m_inputAgregator->initialise();

			m_sdlController->registerKeyboardListener(this);
			m_sdlController->registerMouseListener(this);
		}
		catch (std::exception &e)
		{
			LOGCATEGORY("CEGUI_Service/init").error("Failed to initialize:%s", e.what());
			return ActionResult::AR_ERROR;
		}

		if (!InitializeLayouts())
			return ActionResult::AR_ERROR;
		

		isInitialized = true;
		return ActionResult();
	}

	void GEM::CEGUI_Service::shutdown()
	{
	}

	Service::ActionResult CEGUI_Service::preFrame(double timeDelta)
	{
		if (!InitializeLayouts())
			return ActionResult::AR_ERROR;

		CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(timeDelta);
		for (auto& Layout : m_layoutsVector)
		{
			Layout->PreFrame(timeDelta);
		}
		return ActionResult();
	}

	Service::ActionResult CEGUI_Service::frame(double timeDelta)
	{
		return ActionResult();
	}

	Service::ActionResult CEGUI_Service::postFrame(double timeDelta)
	{
		return ActionResult();
	}
	bool CEGUI_Service::InitializeLayouts()
	{
		while (m_UninitedLayoutsQueue.size() != 0)
		{
			if (!m_UninitedLayoutsQueue.front()->Initialize())
			{
				LOGCATEGORY("CEGUI_Service/InitializeLayouts").error("One of the layouts failed initialization");
				return false;
			}
			m_layoutsVector.push_back(m_UninitedLayoutsQueue.front());
			m_UninitedLayoutsQueue.pop();
		}
		return true;
	}
	void CEGUI_Service::mouseMoved(const SDL_Event & arg)
	{
		
		m_inputAgregator->injectMousePosition(arg.motion.x, arg.motion.y);
		m_inputAgregator->injectMouseWheelChange(arg.wheel.y);
	}
	void CEGUI_Service::mousePressed(const SDL_MouseButtonEvent & arg)
	{
		m_inputAgregator->injectMouseButtonDown(SDLtoCEGUIMouseButton(arg.button));
	}
	void CEGUI_Service::mouseReleased(const SDL_MouseButtonEvent & arg)
	{
		m_inputAgregator->injectMouseButtonUp(SDLtoCEGUIMouseButton(arg.button));
	}
	void CEGUI_Service::textInput(const SDL_TextInputEvent & arg)
	{
	}
	void CEGUI_Service::keyPressed(const SDL_KeyboardEvent & arg)
	{
		m_inputAgregator->injectKeyDown(toCEGUIKey(arg.keysym.scancode));
		m_inputAgregator->injectChar(arg.keysym.sym);
	}
	void CEGUI_Service::keyReleased(const SDL_KeyboardEvent & arg)
	{
		m_inputAgregator->injectKeyUp(toCEGUIKey(arg.keysym.scancode));
	}
}
