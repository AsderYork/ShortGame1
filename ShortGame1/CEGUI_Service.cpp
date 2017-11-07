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
		case SDL_SCANCODE_1: return CEGUI::Key::Scan::One;
		case SDL_SCANCODE_2: return CEGUI::Key::Scan::Two;
		case SDL_SCANCODE_3: return CEGUI::Key::Scan::Three;
		case SDL_SCANCODE_4: return CEGUI::Key::Scan::Four;
		case SDL_SCANCODE_5: return CEGUI::Key::Scan::Five;
		case SDL_SCANCODE_6: return CEGUI::Key::Scan::Six;
		case SDL_SCANCODE_7: return CEGUI::Key::Scan::Seven;
		case SDL_SCANCODE_8: return CEGUI::Key::Scan::Eight;
		case SDL_SCANCODE_9: return CEGUI::Key::Scan::Nine;
		case SDL_SCANCODE_0: return CEGUI::Key::Scan::Zero;
		case SDL_SCANCODE_RETURN: return CEGUI::Key::Scan::Return;
		case SDL_SCANCODE_BACKSPACE: return CEGUI::Key::Scan::Backspace;
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
	void CEGUI_Service::VoteForVisibleCursor()
	{
		if (m_CursorVisibilityVotes == 0){
			CEGUI::System::getSingleton().getDefaultGUIContext().getCursor().setVisible(true);
		}
		m_CursorVisibilityVotes++;
	}
	void CEGUI_Service::VoteAgainstVisibleCursor()
	{
		if (m_CursorVisibilityVotes == 1) {
			CEGUI::System::getSingleton().getDefaultGUIContext().getCursor().setVisible(false);
		}
		m_CursorVisibilityVotes--;
		if (m_CursorVisibilityVotes < 0) { m_CursorVisibilityVotes = 0; }
	}
	int CEGUI_Service::gettmpCursorVotes() const
	{
		return m_CursorVisibilityVotes;
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
			CEGUI::System::getSingleton().getDefaultGUIContext().getCursor().setVisible(false);

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
		
		return ActionResult();
	}

	void GEM::CEGUI_Service::shutdown()
	{
	}

	Service::ActionResult CEGUI_Service::preFrame(float timeDelta)
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

	Service::ActionResult CEGUI_Service::frame(float timeDelta)
	{
		return ActionResult();
	}

	Service::ActionResult CEGUI_Service::postFrame(float timeDelta)
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
		switch (arg.type)
		{
		case SDL_MOUSEWHEEL: {
			m_inputAgregator->injectMouseWheelChange((float)arg.wheel.y);
			break;
		}

		case SDL_MOUSEMOTION: {
			m_inputAgregator->injectMousePosition((float)arg.motion.x, (float)arg.motion.y);
			break;
		}
		}
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
			m_inputAgregator->injectChar(arg.text[0]);
	}
	void CEGUI_Service::keyPressed(const SDL_KeyboardEvent & arg)
	{
		m_inputAgregator->injectKeyDown(toCEGUIKey(arg.keysym.scancode));
	}
	void CEGUI_Service::keyReleased(const SDL_KeyboardEvent & arg)
	{
		m_inputAgregator->injectKeyUp(toCEGUIKey(arg.keysym.scancode));
	}
}
