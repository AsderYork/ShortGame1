#include "stdafx.h"
#include "VisualController.h"

namespace GEM
{

	void VisualController::StartBackgroundInit()
	{
	}

	void VisualController::WaitForInit()
	{
		m_sky.StartVisualization();
		m_fullyInited = true;

		m_gsController->AddFirstTickCallback([this]() {m_camera.TieCamera(m_gsController->m_playerCharacterID, m_gsController); return true; });
	}

	void VisualController::Frame()
	{
		if (m_fullyInited)
		{
			m_objCubical.Frame();
			m_sky.frame();
			m_camera.UpdateCamera();
		}
	}

	VisualController::~VisualController()
	{
		;
	}

}
