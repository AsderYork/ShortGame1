#include "stdafx.h"
#include "CEGUI_Service.h"
#include <CEGUI\CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

namespace GEM
{
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
		}
		catch (std::exception &e)
		{
			LOGCATEGORY("CEGUI_Service/init").error("Failed to initialize:%s", e.what());
			return ActionResult::AR_ERROR;
		}

		for (auto& Layout : m_layoutsVector)
		{
			if (!Layout->Initialize())
			{
				LOGCATEGORY("CEGUI_Service/init").error("One of the layouts failed initialization");
				return ActionResult::AR_ERROR;
			}
		}


		return ActionResult();
	}

	void GEM::CEGUI_Service::shutdown()
	{
	}

	Service::ActionResult CEGUI_Service::preFrame(double timeDelta)
	{
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
}
