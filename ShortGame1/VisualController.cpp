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
	}

	void VisualController::Frame()
	{
		if (m_fullyInited)
		{
			m_objCubical.Frame();
			m_sky.frame();
		}
	}

	VisualController::~VisualController()
	{
		;
	}

}
