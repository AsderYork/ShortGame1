#include "stdafx.h"
#include "VisualController.h"

namespace GEM
{
	VisualController::VisualController(GameSimController * gsController) :
		m_gsController(gsController),
		m_landscape(gsController),
		m_objCubical(gsController),
		m_sky(gsController),
		m_objSimplistic(gsController)
	{}

	void VisualController::StartBackgroundInit()
	{
	}

	void VisualController::WaitForInit()
	{
		m_sky.StartVisualization();
		m_fullyInited = true;

		m_gsController->AddFirstTickCallback([this]() {
			auto PlayerPtr = m_gsController->m_entities.GetEntity(m_gsController->m_playerCharacterID);
			if (PlayerPtr.expired())
			{
				LOGCATEGORY("VisualController/WaitForInitCallback").error("Can't tie to a player! Entity is invalid");
				return false;
			}
			m_camera.TieCamera(PlayerPtr);
			return true;
		});
	}

	void VisualController::Frame(float TimePassed)
	{
		if (m_fullyInited)
		{
			//m_objCubical.Frame();
			m_sky.frame();
			m_camera.UpdateCamera();
			m_objSimplistic.frame(TimePassed);
		}
	}

	VisualController::~VisualController()
	{
		;
	}

}
